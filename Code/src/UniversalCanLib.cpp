#include <ProtocolDefinitionParser.h>
#include <Transcievers/ELM327.h>
#include <iostream>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WINDOWS
#endif

#ifdef WINDOWS
#include <Transcievers/SerialWindows.h>
#else
#include <Transcievers/SerialUnix.h>
#endif

extern "C" void init_universal_can(std::string port_name)
{
    std::cout << "init_universal_can: " << port_name << std::endl;
    return;
    auto serial = std::make_unique<SerialWindows>();
    if (!serial->open(port_name)) {
        throw std::runtime_error("Failed to open serial port");
    }
    auto parser = std::make_unique<ProtocolDefinitionParser>("ProtocolDefinitions/J1979.xml");
    ELM327 elm327(std::move(serial), std::move(parser));
    
    elm327.start();

    while (true) {
        if (elm327.messageAvailable()) {
            auto message = elm327.readMessage();
            message->print();
        }
    };

}