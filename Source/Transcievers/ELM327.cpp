#include "ELM327.h"
#include "ISerial.h"

ELM327::ELM327(std::unique_ptr<ISerial> serial) 
    : serial(std::move(serial)), running(false) {
        serial.registerCallback(serialReceiveCallback);
        serial.write("AT Z\n");
            
    }

ELM327::~ELM327() {
    running = false;
    serial->close();
}

void ELM327::serialReceiveCallback(std::vector<uint8_t> message) {
    if(message == "ELM327 v2.0\n>") {
        this.running = true;
    }
}

bool ELM327::sendMessage(std::vector<CanMessage> messages) {
    for (const auto& msg : messages) {
        std::vector<uint8_t> data(reinterpret_cast<const uint8_t*>(&msg), 
                                  reinterpret_cast<const uint8_t*>(&msg) + sizeof(msg));
        if (!serial->write(data)) {
            return false;
        }
    }
    return true;
}

void ELM327::registerCallback(std::function<void(CanMessage)> callback) {
    callbackFunction = callback;
    running = true;

    serial->registerCallback([this](const std::vector<uint8_t>& data) {
        CanMessage message;
        if (data.size() == sizeof(CanMessage)) {
            memcpy(&message, data.data(), sizeof(CanMessage));
            if (callbackFunction) {
                callbackFunction(message);
            }
        }
    });
}
