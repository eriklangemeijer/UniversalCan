#pragma once
#include <string>
#include <vector>
#include <functional>
#include "CanMessage.h"

class ISerial {
public:
    virtual bool open(const std::string& port) = 0;
    virtual void close() = 0;
    virtual bool write(const std::vector<uint8_t>& data) = 0;
    virtual bool writeString(std::string data) = 0;
    virtual std::string read() = 0;
    virtual void registerCallback(std::function<void(std::string)> callback) = 0;
};
