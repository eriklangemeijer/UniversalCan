#include "ProtocolDefinitionParser.h"
#include <Transcievers/ELM327.h>
#include <Transcievers/ISerial.h>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

const uint16_t response_wait_sleep_time_ns = 10;
const uint16_t response_wait_timeout_ms = 50;

ELM327::ELM327(std::unique_ptr<ISerial> serial, std::unique_ptr<ProtocolDefinitionParser> protocol_parser) 
    : serial(std::move(serial)), protocol_parser(std::move(protocol_parser)), running(false), ready(false) {
            
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


void ELM327::serialReceiveCallback(std::vector<uint8_t> message) {
    // std::cout << "REC:" << message << std::endl;
    std::string const message_str(message.begin(), message.end());
    if((message_str.rfind("ELM327 v", 0) == 0)) {
        this->running = true;
    }
    this->ready = message.back() == '>'; 


    auto msg_template = this->protocol_parser->findMatch(message);
    auto can_message = CanMessage(message, msg_template);

    storeMessage(can_message);
}

bool ELM327::sendMessage(std::vector<CanMessage> messages) {
    for (const auto& msg : messages) {
      if (!serial->write(msg.data)) {
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
    
    serial->registerCallback([this](std::vector<uint8_t> message) {
        this->serialReceiveCallback(message);
    });
    this->callbackFunction = callback;
    this->running = true;

}

bool ELM327::messageAvailable() {
    this->messageListLock.lock();
    bool const available = !this->messageList.empty();
    this->messageListLock.unlock();
    return available;
}

void ELM327::storeMessage(CanMessage& message) {
    this->messageListLock.lock();
    this->messageList.push_back(message);
    this->messageListLock.unlock();

}

std::shared_ptr<CanMessage> ELM327::readMessage() {
    std::shared_ptr<CanMessage> message = nullptr;
    this->messageListLock.lock();
    if (!this->messageList.empty()) {
      message = std::make_unique<CanMessage>(this->messageList.front());
      this->messageList.pop_front();
    }
    this->messageListLock.unlock();
    return message;
}