#include "ProtocolDefinitionParser.h"
#include <Transcievers/ELM327.h>
#include <Transcievers/ISerial.h>
#include <algorithm>
#include <sstream>
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
#include <string>
#include <iostream>
#include <iomanip>
const uint16_t response_wait_sleep_time_ns = 10;
const uint16_t response_wait_timeout_ms = 500;

ELM327::ELM327(std::unique_ptr<ISerial> serial, std::unique_ptr<ProtocolDefinitionParser> protocol_definition)
    : serial(std::move(serial)), protocol_definition(std::move(protocol_definition)), running(false), ready(false) {

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
    sendATMessage("E0", true); // Echo off
    // sendATMessage("L0", true); // Linefeeds off
    sendATMessage("SP0", true); // Set protocol to auto

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
    std::string const message_str(message.begin(), message.end());
    if ((message_str.rfind("ELM327 v", 0) == 0)) {
        this->running = true;
    }
    if(message.back() == '>') {
        this->ready = true;
    }

    bool const is_hex = std::all_of(message.begin(), message.end(), [](char character) {
        return std::isxdigit(static_cast<unsigned char>(character)) || character == ' ';
    });

    if (is_hex) {
        std::cout << "Received hex message: " << message_str << std::endl;
        std::vector<uint8_t> data = hexStringToBytes(message_str); 
        auto msg_template = this->protocol_definition->findMatch(data);

        auto can_message = CanMessage(data, msg_template);
        storeMessage(can_message);
    } else {
        std::cout << "Received non-hex message: " << message_str << std::endl;
    }
}

bool ELM327::sendMessages(std::vector<CanMessage> messages) {
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

bool ELM327::requestMessage(std::string msg_name) {
    auto msg_template = this->protocol_definition->findMessageByName(msg_name);
    if (!msg_template) {
        std::cerr << "Message not found: " << msg_name << std::endl;
        return false;
    }
    std::vector<uint8_t> request_data = msg_template->getRequestMessage();
    std::stringstream msg_stream;
    msg_stream << std::hex << std::uppercase << std::setfill('0');
    for (size_t ii = 0; ii < request_data.size(); ii++) {
        msg_stream << std::setw(2) << static_cast<unsigned>(request_data[ii]);
        if (ii == request_data.size()-1) {
            msg_stream << "\r";
        } else {
            msg_stream << " ";
        }
    }
    std::string msg_string = msg_stream.str();
    this->serial->writeString(msg_string);
    return true;
}

bool ELM327::messageAvailable() {
    this->message_list_lock.lock();
    const bool available = !(this->message_list.empty());
    this->message_list_lock.unlock();
    return available;
}

void ELM327::storeMessage(CanMessage &message) {
    this->message_list_lock.lock();
    this->message_list.push_back(message);
    this->message_list_lock.unlock();
}

std::shared_ptr<CanMessage> ELM327::readMessage() {
    std::shared_ptr<CanMessage> message = nullptr;
    this->message_list_lock.lock();
    if (!this->message_list.empty()) {
        message = std::make_unique<CanMessage>(this->message_list.front());
        this->message_list.pop_front();
    }
    this->message_list_lock.unlock();
    return message;
}