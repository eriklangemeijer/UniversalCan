#pragma once
#include <string>
#include <vector>
#include <functional>
#include "CanMessage.h"
const uint16_t SERIAL_READ_BUFFER_SIZE = 1024;
class ISerial {
public:
    virtual ~ISerial() = default; // Virtual destructor

    virtual bool open(const std::string& port) = 0;
    virtual void close() = 0;
    virtual bool write(const std::vector<uint8_t>& data) = 0;
    virtual bool writeString(std::string data) = 0;
    virtual std::string read() = 0;
    virtual void registerCallback(std::function<void(std::string)> callback) = 0;
};
