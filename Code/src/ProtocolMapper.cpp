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
#include <stdexcept>

int main() {
    try {
#ifdef WINDOWS
        auto serial = std::make_unique<SerialWindows>();
        const std::string port_name = "COM9";
#else
        auto serial = std::make_unique<SerialUnix>();
        const std::string port_name = "/dev/tty.usbserial-110";
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

    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
