#include "ELM327.h"
#include "SerialWindows.h"
#include <stdexcept>

void function_name(){
    
}

int main() {
    try {
        auto serial = std::make_unique<SerialWindows>();
        if (!serial->open("COM3")) {
            throw std::runtime_error("Failed to open serial port");
        }

        ELM327 elm327(std::move(serial));

        elm327.registerCallback([](const CanMessage& msg) {
            // Handle incoming CAN message
            printf("Received CAN message\n");
        });

        // Send a test message
        std::vector<CanMessage> messages = { /* populate messages */ };
        elm327.sendMessage(messages);

    } catch (const std::exception& e) {
        printf("Error: %s\n", e.what());
    }

    return 0;
}
