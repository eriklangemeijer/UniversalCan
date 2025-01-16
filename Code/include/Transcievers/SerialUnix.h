#pragma once

#include <Transcievers/ISerial.h>
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class SerialUnix : public ISerial {
private:
    void readLoop();

    int fd_;
    std::mutex mutex_;
    std::mutex callbackMutex_;
    bool running;
    std::function<void(std::vector<uint8_t>)> callbackPtr;
    void threadFunction();
public:
    SerialUnix();
    ~SerialUnix();

    bool open(const std::string& port) override;
    void close() override;
    bool writeString(std::string data) override;
    bool write(const std::vector<uint8_t>& data) override;
    std::string read() override;
    void registerCallback(std::function<void(std::vector<uint8_t>)> callback) override;
};

