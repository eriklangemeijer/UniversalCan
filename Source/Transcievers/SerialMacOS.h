#pragma once

#include "ISerial.h"
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class SerialMacOS : public ISerial {
public:
    SerialMacOS();
    ~SerialMacOS();

    bool open(const std::string& port);
    void close();
    bool write(const std::vector<uint8_t>& data);
    bool read(std::vector<uint8_t>& buffer, size_t size);
    void registerCallback(std::function<void(std::vector<uint8_t>)> callback);

private:
    void readLoop();

    int fd_;
    std::atomic<bool> running_;
    std::thread readerThread_;
    std::mutex mutex_;
    std::mutex callbackMutex_;
    std::function<void(std::vector<uint8_t>)> callback_;
};

