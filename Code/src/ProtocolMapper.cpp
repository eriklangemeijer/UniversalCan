#include <Transcievers/ELM327.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define WINDOWS
#endif

#ifdef WINDOWS
    #include <Transcievers/SerialWindows.h>
#elif __APPLE__
    #include <Transcievers/SerialMacOS.h>
#endif

#include <stdexcept>
#include <iostream>
#include <memory> 

void canMessageReceiveCallback(std::string message) {
    if(message == "ELM327 v2.0\r\r>" or message == "ELM327 v1.3a\r\r>") {
        return;
    }
}

int main() {
    try {
        #ifdef WINDOWS
            auto serial = std::make_unique<SerialWindows>();
            const std::string port_name = "COM9";
        #elif __APPLE__
            auto serial = std::make_unique<SerialMacOS>();
            const std::string port_name = "/dev/tty.usbserial-10";
        #endif
        
        if (!serial->open(port_name)) {
            throw std::runtime_error("Failed to open serial port");
        }
        ELM327 elm327(std::move(serial));

        elm327.registerCallback([](const CanMessage& msg) {
            // Handle incoming CAN message
            std::cout << "Received CAN message\n";
        });
        elm327.start();
        // // Send a test message
        // std::vector<CanMessage> messages = { /* populate messages */ };
        // elm327.sendMessage(messages);

        while(true) {
            if(elm327.messageAvailable())
            {
                std::string message = elm327.readMessage();
                std::cout << message.c_str() << std::endl;
            }
        };

    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
