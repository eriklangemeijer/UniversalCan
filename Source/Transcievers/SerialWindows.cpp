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

    comParams.BaudRate = CBR_9600;
    comParams.ByteSize = 8;
    comParams.StopBits = ONESTOPBIT;
    comParams.Parity = NOPARITY;
    comParams.fDtrControl = DTR_CONTROL_ENABLE;
    comParams.EvtChar = '\n';

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

bool SerialWindows::write(const std::vector<uint8_t>& data) {
    if (comPort == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesWritten;
    return WriteFile(comPort, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, NULL);
}

bool SerialWindows::read(std::vector<uint8_t>& buffer, size_t size) {
    if (comPort == INVALID_HANDLE_VALUE) {
        return false;
    }

    buffer.resize(size);
    DWORD bytesRead;
    bool result = ReadFile(comPort, buffer.data(), static_cast<DWORD>(size), &bytesRead, NULL);
    buffer.resize(bytesRead); // Adjust size to actual bytes read
    return result;
}

void SerialWindows::registerCallback(std::function<void(std::vector<uint8_t>)> callback) {
    this->callback = callback;
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
            ReadFile(comPort, buffer.data(), buffer.size(), &bytesRead, &ov);
            buffer.resize(bytesRead); // Adjust size to actual bytes read
            if (callback) {
                callback(buffer);
            }
        }
    }

    CloseHandle(ov.hEvent);
}
