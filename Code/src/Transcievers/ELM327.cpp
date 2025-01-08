#include <Transcievers/ELM327.h>
#include <Transcievers/ISerial.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

const uint16_t response_wait_sleep_time_ns = 10;
const uint16_t response_wait_timeout_ms = 50;

ELM327::ELM327(std::unique_ptr<ISerial> serial) 
    : serial(std::move(serial)), running(false), ready(false) {
            
    }

ELM327::~ELM327() {
    this->running = false;
    this->serial->close();
}

void ELM327::start() {
    sendATMessage("L0");
    sendATMessage("E0");
    sendATMessage("Z");
    sendATMessage("RV");
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

bool ELM327::sendATMessage(std::string command, bool waitForResponse) {
    this->serial->writeString("AT" + command + "\r");

    if (waitForResponse) {
        auto start_time = std::chrono::steady_clock::now();
        while (!ready) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(response_wait_sleep_time_ns));
            auto elapsed_time = std::chrono::steady_clock::now() - start_time;
            if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() >= response_wait_timeout_ms) {
                return false;
            }
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