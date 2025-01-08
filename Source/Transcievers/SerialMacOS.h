#pragma once

#include "ISerial.h"
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class SerialMacOS : public ISerial {
private:
    void readLoop();

    int fd_;
    std::mutex mutex_;
    std::mutex callbackMutex_;
    bool running;
    std::function<void(std::string)> callbackPtr;
    void threadFunction();
public:
    SerialMacOS();
    ~SerialMacOS();

    bool open(const std::string& port) override;
    void close() override;
    bool writeString(std::string data) override;
    bool write(const std::vector<uint8_t>& data) override;
    std::string read() override;
    void registerCallback(std::function<void(std::string)> callback) override;
};

