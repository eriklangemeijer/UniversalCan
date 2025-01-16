#pragma once
#include "CanMessage.h"
#include <functional>
#include <string>
#include <vector>
const uint16_t SERIAL_READ_BUFFER_SIZE = 1024;
class ISerial {
  public:
    virtual ~ISerial() = default; // Virtual destructor

    virtual bool open(const std::string &port) = 0;
    virtual void close() = 0;
    virtual bool write(const std::vector<uint8_t> &data) = 0;
    virtual bool writeString(std::string data) = 0;
    virtual std::string read() = 0;
    virtual void registerCallback(std::function<void(std::vector<uint8_t>)> callback) = 0;
};
