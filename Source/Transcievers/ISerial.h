#pragma once
#include <string>
#include <vector>
#include <functional>
#include "CanMessage.h"

class ISerial {
public:
    virtual ~ISerial() = default;

    virtual bool open(const std::string& port) = 0;
    virtual void close() = 0;
    virtual bool write(const std::vector<uint8_t>& data) = 0;
    virtual bool read(std::vector<uint8_t>& buffer, size_t size) = 0;
    virtual void registerCallback(std::function<void(std::vector<uint8_t>)> callback) = 0;
};
