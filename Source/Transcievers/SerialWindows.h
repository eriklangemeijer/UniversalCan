#pragma once

#include "ISerial.h"
#include <windows.h>
#include <thread>
#include <functional>
#include <vector>
#include <string>

class SerialWindows : public ISerial {
private:
    HANDLE comPort;
    bool running;
    std::function<void(std::vector<uint8_t>)> callback;

    // Internal thread function to handle incoming data
    void threadFunction();

public:
    SerialWindows();
    ~SerialWindows() override;

    bool open(const std::string& port) override;
    void close() override;
    bool write(const std::vector<uint8_t>& data) override;
    bool read(std::vector<uint8_t>& buffer, size_t size) override;
    void registerCallback(std::function<void(std::vector<uint8_t>)> callback) override;
};
