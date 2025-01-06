#include "SerialWindows.h"

SerialWindows::SerialWindows() 
    : comPort(INVALID_HANDLE_VALUE), running(false) {}

SerialWindows::~SerialWindows() {
    close();
}

bool SerialWindows::open(const std::string& port) {
    comPort = CreateFileA(port.c_str(),
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    if (comPort == INVALID_HANDLE_VALUE) {
        return false;
    }

    DCB comParams = { 0 };
    if (!GetCommState(comPort, &comParams)) {
        return false;
    }

    comParams.BaudRate = CBR_38400;
    comParams.ByteSize = 8;
    comParams.StopBits = ONESTOPBIT;
    comParams.Parity = NOPARITY;
    comParams.fDtrControl = DTR_CONTROL_ENABLE;
    comParams.EvtChar = '\r';



    if (!SetCommState(comPort, &comParams)) {
        return false;
    }

    PurgeComm(comPort, PURGE_RXCLEAR | PURGE_TXCLEAR);
    return true;
}

void SerialWindows::close() {
    running = false;
    if (comPort != INVALID_HANDLE_VALUE) {
        CloseHandle(comPort);
        comPort = INVALID_HANDLE_VALUE;
    }
}

bool SerialWindows::writeString(std::string data) {
    // Convert the string to a vector of uint8_t
    std::vector<uint8_t> byteData(data.begin(), data.end());

    // Call the `write` function with the converted data
    return write(byteData);
}

bool SerialWindows::write(const std::vector<uint8_t>& data) {
    if (comPort == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesWritten;
    return WriteFile(comPort, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, NULL);
}

std::string SerialWindows::read() {
    if (comPort == INVALID_HANDLE_VALUE) {
        return "";
    }
    std::vector<uint8_t> buffer(1024); // Example buffer size
    unsigned long bytesRead;
    bool ret = ReadFile(comPort, buffer.data(), buffer.size(), &bytesRead, NULL);
    if(bytesRead > 0) {
        buffer.resize(bytesRead); // Adjust size to actual bytes read
        if (this->callbackPtr) {
            std::string str;
            str.assign(buffer.begin(), buffer.end());
            return str;
        }
    }
    
    return "";
}

void SerialWindows::registerCallback(std::function<void(std::string)> callback) {
    this->callbackPtr = callback;
    running = true;
    std::thread(&SerialWindows::threadFunction, this).detach();
}

void SerialWindows::threadFunction() {
    OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(0, true, 0, 0);
    unsigned long reason;

    while (running) {
        WaitCommEvent(comPort, &reason, &ov);
        if (WaitForSingleObject(ov.hEvent, INFINITE) == WAIT_OBJECT_0) {
            std::vector<uint8_t> buffer(1024); // Example buffer size
            unsigned long bytesRead;
            bool ret = ReadFile(comPort, buffer.data(), buffer.size(), &bytesRead, &ov);
            if(bytesRead > 0) {
                buffer.resize(bytesRead); // Adjust size to actual bytes read
                if (this->callbackPtr) {
                    std::string str;
                    str.assign(buffer.begin(), buffer.end());
                    this->callbackPtr(std::move(str));
                }
            }
        }
    }

    CloseHandle(ov.hEvent);
}
