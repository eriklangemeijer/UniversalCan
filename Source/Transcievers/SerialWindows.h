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
    ~SerialWindows();

};
