#include "ELM327.h"
#include "ISerial.h"
#include <iostream>


ELM327::ELM327(std::unique_ptr<ISerial> serial) 
    : serial(std::move(serial)), running(false) {
            
    }

ELM327::~ELM327() {
    running = false;
    serial->close();
}

void ELM327::start() {
    serial->writeString("ATZ\r");

}


void ELM327::serialReceiveCallback(std::string message) {
    // std::cout << "REC:" << message << std::endl;
    if(message == "ELM327 v2.0\r\r>" or message == "\r\rELM327 v1.3a\r\r>") {
        this->running = true;
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
    
    serial->registerCallback([this](std::string message) {
        this->serialReceiveCallback(message);
    });
    this->callbackFunction = callback;
    running = true;

}
