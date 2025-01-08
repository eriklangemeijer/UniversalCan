#include "ELM327.h"
#include "ISerial.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

ELM327::ELM327(std::unique_ptr<ISerial> serial) 
    : serial(std::move(serial)), running(false), ready(false) {
            
    }

ELM327::~ELM327() {
    this->running = false;
    this->serial->close();
}

void ELM327::start() {
    this->serial->writeString("AT L0\r");
    this->serial->writeString("AT E0\r");
    this->serial->writeString("AT Z\r");
    while(!ready) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(10));
    }
    this->serial->writeString("AT RV\r");
    // serial->writeString("AT DP\r");
    // serial->writeString("AT MA\r");

}


void ELM327::serialReceiveCallback(const std::string& message) {
    // std::cout << "REC:" << message << std::endl;
    if((message.rfind("ELM327 v", 0) == 0)) {
        this->running = true;
    }
    this->ready = message.back() == '>'; 
    storeMessage(message);
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
    
    serial->registerCallback([this](const std::string& message) {
        this->serialReceiveCallback(message);
    });
    this->callbackFunction = callback;
    this->running = true;

}

bool ELM327::messageAvailable() {
    this->messageListLock.lock();
    bool available = this->messageList.size() > 0;
    this->messageListLock.unlock();
    return available;
}

void ELM327::storeMessage(std::string message) {
    this->messageListLock.lock();
    this->messageList.push_back(message);
    this->messageListLock.unlock();

}

std::string ELM327::readMessage() {
    std::string message = "";
    this->messageListLock.lock();
    if(this->messageList.size() > 0) {
        message = this->messageList.front();
        this->messageList.pop_front();
    }
    this->messageListLock.unlock();
    return message;
}