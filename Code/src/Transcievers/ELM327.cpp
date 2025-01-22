#include "ProtocolDefinitionParser.h"
#include <Transcievers/ELM327.h>
#include <Transcievers/ISerial.h>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include <iostream>
const uint16_t response_wait_sleep_time_ns = 10;
const uint16_t response_wait_timeout_ms = 500;

ELM327::ELM327(std::unique_ptr<ISerial> serial, std::unique_ptr<ProtocolDefinitionParser> protocol_parser)
    : serial(std::move(serial)), protocol_parser(std::move(protocol_parser)), running(false), ready(false) {

}

ELM327::~ELM327() {
    this->running = false;
    this->serial->close();
}

void ELM327::start() {
    serial->registerCallback([this](std::vector<uint8_t> message) {
        this->serialReceiveCallback(message);
    });
    sendATMessage("Z", true);  // Reset
    // sendATMessage("E0", true); // Echo off
    // sendATMessage("L0", true); // Linefeeds off
    sendATMessage("SP0", true); // Linefeeds off

    this->serial->writeString("01 00\r");

    // sendATMessage("MA"); // Monitor All
}

std::vector<uint8_t> ELM327::hexStringToBytes(std::string message_str) {

    std::vector<uint8_t> bytes_vector;
    //Hex strings are formated "XX XX XX..." so we look at two digits and then skip the space
    for (unsigned int ii = 0; ii < message_str.length(); ii += 3) {
        std::string const byte_string = message_str.substr(ii, 2);
        uint8_t const byte = (uint8_t)strtol(byte_string.c_str(), nullptr, 16);
        bytes_vector.push_back(byte);
    }
    return bytes_vector;
}

void ELM327::serialReceiveCallback(std::vector<uint8_t> message) {
    // std::cout << "REC:" << message << std::endl;
    std::string const message_str(message.begin(), message.end());
    if ((message_str.rfind("ELM327 v", 0) == 0)) {
        this->running = true;
    }
    if(message.back() == '>') {
        this->ready = true;
    }

    auto msg_template = this->protocol_parser->findMatch(message);
    bool const is_hex = std::all_of(message.begin(), message.end(), [](char character) {
        return std::isxdigit(static_cast<unsigned char>(character)) || character == ' ';
    });

    if (is_hex) {
        std::vector<uint8_t> data = hexStringToBytes(message_str); 
        auto can_message = CanMessage(data, msg_template);
        storeMessage(can_message);
    } else {
        std::cout << "Received non-hex message: " << message_str << std::endl;
    }
}

bool ELM327::sendMessage(std::vector<CanMessage> messages) {
    for (const auto &msg : messages) {
        if (!serial->write(msg.data)) {
            return false;
        }
    }
    return true;
}

bool ELM327::sendATMessage(std::string command, bool wait_for_response) {
    this->ready = false;
    this->serial->writeString("AT " + command + "\r");

    if (wait_for_response) {
        auto start_time = std::chrono::steady_clock::now();
        while (!ready) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(response_wait_sleep_time_ns));
            auto elapsed_time = std::chrono::steady_clock::now() - start_time;
            if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() >= response_wait_timeout_ms) {
                std::cerr << "Timeout waiting for response to " << command << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool ELM327::messageAvailable() {
    this->messageListLock.lock();
    bool const available = !this->messageList.empty();
    this->messageListLock.unlock();
    return available;
}

void ELM327::storeMessage(CanMessage &message) {
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