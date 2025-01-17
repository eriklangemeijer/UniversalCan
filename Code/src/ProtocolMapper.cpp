#include <CanMessage.h>
#include <ProtocolDefinitionParser.h>
#include <Transcievers/ELM327.h>
#include <exception>
#include <string>
#include <utility>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WINDOWS
#endif

#ifdef WINDOWS
#include <Transcievers/SerialWindows.h>
#else
#include <Transcievers/SerialUnix.h>
#endif

#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>

bool isValidPortName(const std::string &port_name) {
#ifdef _WIN32
    std::regex windows_regex("^COM[1-9][0-9]*$");
    return std::regex_match(port_name, windows_regex);
#else
    std::regex unix_regex("^/dev/tty\\..+$");
    return std::regex_match(port_name, unix_regex);
#endif
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <port_name>" << std::endl;
        return 1;
    }

    std::string port_name = argv[1];

    if (!isValidPortName(port_name)) {
        std::cerr << "Invalid port name: " << port_name << std::endl;
        return 1;
    }

#ifdef WINDOWS
    auto serial = std::make_unique<SerialWindows>();
#else
    auto serial = std::make_unique<SerialUnix>();
#endif

    if (!serial->open(port_name)) {
        throw std::runtime_error("Failed to open serial port");
    }
    auto parser = std::make_unique<ProtocolDefinitionParser>("../../../ProtocolDefinitions/J1979.xml");
    ELM327 elm327(std::move(serial), std::move(parser));
    elm327.registerCallback([](const CanMessage & /*msg*/) {
        // Handle incoming CAN message
        std::cout << "Received CAN message\n";
    });
    elm327.start();

    while (true) {
        if (elm327.messageAvailable()) {
            auto message = elm327.readMessage();
            message->print();
        }
    };

    return 0;
}
