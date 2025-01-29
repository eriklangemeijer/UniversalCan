#include <ProtocolDefinitionParser.h>
#include <Transcievers/ELM327.h>
#include <iostream>
#include <string>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WINDOWS
#endif

#ifdef WINDOWS
#include <Transcievers/SerialWindows.h>
#else
#include <Transcievers/SerialUnix.h>
#endif

typedef void (*callback_t)(const char*);

extern "C" ELM327* universal_can_init(const char* port_name, const char* protocol_definition_path)
{
    ELM327* elm327 = nullptr;
    std::cout << "init_universal_can: " << port_name << std::endl;

    std::string port_string(port_name);
    auto serial = std::make_unique<SerialWindows>();
    if (!serial->open(port_string)) {
        std::cerr << "Failed to open serial port " << port_string << std::endl;
        return elm327;
    }
    try
    {
        auto parser = std::make_unique<ProtocolDefinitionParser>(std::string(protocol_definition_path));
        elm327 = new ELM327(std::move(serial), std::move(parser));        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return elm327;
}

extern "C" void universal_can_delete(ELM327* elm327)
{
    delete elm327;
}

extern "C" void universal_can_run_loop(ELM327* elm327, callback_t callback)
{
    elm327->start();

    while (true) {
        if (elm327->messageAvailable()) {
            auto message = elm327->readMessage();
            message->print();
            callback(message->to_string().c_str());
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

}