#include <ELM327.h>
#include "SerialWindows.h"
#include <stdexcept>

void canMessageReceiveCallback(std::string message) {
    if(message == "ELM327 v2.0\r\r>" or message == "ELM327 v1.3a\r\r>") {
        return;
    }
}

int main() {
    try {
        auto serial = std::make_unique<SerialWindows>();
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
            printf("Received CAN message\n");
        });
        elm327.start();
        // // Send a test message
        // std::vector<CanMessage> messages = { /* populate messages */ };
        // elm327.sendMessage(messages);

        while(true) {};

    } catch (const std::exception& e) {
        printf("Error: %s\n", e.what());
    }

    return 0;
}
