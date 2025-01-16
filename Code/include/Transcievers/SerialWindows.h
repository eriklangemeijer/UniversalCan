#pragma once

#include <Transcievers/ISerial.h>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

class SerialWindows : public ISerial {
  private:
    HANDLE comPort;
    bool running;
    std::function<void(std::vector<uint8_t>)> callbackPtr;

    // Internal thread function to handle incoming data
    void threadFunction();

  public:
    SerialWindows();
    ~SerialWindows();

    bool open(const std::string &port) override;
    void close() override;
    bool writeString(std::string data) override;
    bool write(const std::vector<uint8_t> &data) override;
    std::string read() override;
    void registerCallback(std::function<void(std::vector<uint8_t>)> callback) override;
};
