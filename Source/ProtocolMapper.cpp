#include <ELM327.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define WINDOWS
#endif

#if WINDOWS
#include "SerialWindows.h"
#elif __APPLE__
#include "SerialMacOS.h"
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
        #if WINDOWS
        auto serial = std::make_unique<SerialWindows>();
        #elif __APPLE__
        auto serial = std::make_unique<SerialMacOS>();
        #endif
        
        if (!serial->open("COM9")) {
            throw std::runtime_error("Failed to open serial port");
        }
        // serial->registerCallback(serialReceiveCallback);
        // serial->writeString("ATZ\r");
        
        std::vector<uint8_t> buffer(1024); // Example buffer size
        // serial->read(buffer, buffer.size());
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
        printf("Error: %s\n", e.what());
    }

    return 0;
}
