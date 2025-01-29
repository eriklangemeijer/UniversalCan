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
#ifdef WINDOWS
    auto serial = std::make_unique<SerialWindows>();
#else
    auto serial = std::make_unique<SerialUnix>();
#endif
    if (!serial->open(port_string)) {
        std::cerr << "Failed to open serial port " << port_string << std::endl;
        return elm327;
    }
    try
    {
        auto parser = std::make_unique<ProtocolDefinitionParser>(std::string(protocol_definition_path));
        elm327 = new ELM327(std::move(serial), std::move(parser));       
        
        elm327->start(); 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    std::cout << "returning elm327 " << elm327 << std::endl;
    return elm327;
}

extern "C" void universal_can_delete(ELM327* elm327)
{
    std::cout << "Deleting elm327" << std::endl;
    delete elm327;
}

extern "C" void universal_can_check_message(ELM327* elm327, callback_t callback)
{
    if(!elm327) {
        std::cout << "elm327 pointer not initialised" << std::endl;
        return;
    }

    if (elm327->messageAvailable()) {
        auto message = elm327->readMessage();
        message->print();
        callback(message->to_string().c_str());
    }

}
extern "C" void universal_can_request_msg(ELM327* elm327, const char* msg_name)
{
    if(!elm327) {
        std::cout << "elm327 pointer not initialised" << std::endl;
        return;
    }
    elm327->requestMessage(msg_name);
}
