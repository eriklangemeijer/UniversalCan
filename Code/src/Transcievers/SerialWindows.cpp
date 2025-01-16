#include <Transcievers/SerialWindows.h>
#include <fileapi.h>
#include <cstddef>
#include <handleapi.h>
#include <minwindef.h>
#include <functional>
#include <minwinbase.h>
#include <stdexcept>
#include <string>
#include <winnt.h>
#include <windows.h>
#include <synchapi.h>

SerialWindows::SerialWindows()
    : comPort(INVALID_HANDLE_VALUE), running(false) {}

SerialWindows::~SerialWindows() {
    close();
}

bool SerialWindows::open(const std::string &port) {
    comPort = CreateFileA(port.c_str(),
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          nullptr,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          nullptr);
    if (comPort == INVALID_HANDLE_VALUE) {
        return false;
    }

    DCB com_params = {0};
    if (!GetCommState(comPort, &com_params)) {
        return false;
    }

    com_params.BaudRate = CBR_38400;
    com_params.ByteSize = 8;
    com_params.StopBits = ONESTOPBIT;
    com_params.Parity = NOPARITY;
    com_params.fDtrControl = DTR_CONTROL_ENABLE;
    com_params.EvtChar = '\r';

    if (!SetCommState(comPort, &com_params)) {
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
    if (data.rfind("AT", 0) != 0) {
        throw std::runtime_error("Trying to send something that is not an AT command. To prevent self harm this is currently not possible");
    }
    // Convert the string to a vector of uint8_t
    std::vector<uint8_t> byte_data(data.begin(), data.end());

    // Call the `write` function with the converted data
    return write(byte_data);
}

bool SerialWindows::write(const std::vector<uint8_t> &data) {
    if (comPort == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytes_written = 0;
    return WriteFile(comPort, data.data(), static_cast<DWORD>(data.size()), &bytes_written, NULL);
}

std::string SerialWindows::read() {
    if (comPort == INVALID_HANDLE_VALUE) {
        return "";
    }
    std::vector<uint8_t> buffer(SERIAL_READ_BUFFER_SIZE); // Example buffer size
    unsigned long bytes_read = 0;
    ReadFile(comPort, buffer.data(), buffer.size(), &bytes_read, NULL);
    if (bytes_read > 0) {
        buffer.resize(bytes_read); // Adjust size to actual bytes read
        if (this->callbackPtr) {
            std::string str;
            str.assign(buffer.begin(), buffer.end());
            return str;
        }
    }

    return "";
}

void SerialWindows::registerCallback(std::function<void(std::vector<uint8_t>)> callback) {
    this->callbackPtr = callback;
    running = true;
    std::thread(&SerialWindows::threadFunction, this).detach();
}

void SerialWindows::threadFunction() {
    OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(nullptr, true, 0, nullptr);
    std::vector<uint8_t> rolling_buffer;
    while (this->running) {
        WaitCommEvent(comPort, nullptr, &ov);
        if (WaitForSingleObject(ov.hEvent, INFINITE) == WAIT_OBJECT_0) {
            std::vector<uint8_t> buffer(SERIAL_READ_BUFFER_SIZE); // Example buffer size
            unsigned long bytes_read = 0;
            ReadFile(comPort, buffer.data(), buffer.size(), &bytes_read, &ov);
            if (bytes_read > 0) {
                for (uint8_t character : buffer) {
                    if (character == '\r') {
                        if (!(rolling_buffer.empty())) {
                            this->callbackPtr(rolling_buffer);
                            rolling_buffer.clear();
                        }
                    } else if (character != 0) {
                        rolling_buffer.push_back(character);
                    }
                }
            }
        }
    }

    CloseHandle(ov.hEvent);
}
