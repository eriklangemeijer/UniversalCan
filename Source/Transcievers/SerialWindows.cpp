#include "SerialWindows.h"
#include <stdexcept>

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

    DCB com_params = { 0 };
    if (!GetCommState(comPort, &comParams)) {
        return false;
    }

    comParams.BaudRate = CBR_38400;
    com_params.ByteSize = 8;
    comParams.StopBits = ONESTOPBIT;
    comParams.Parity = NOPARITY;
    comParams.fDtrControl = DTR_CONTROL_ENABLE;
    com_params.EvtChar = '\r';

// COMMTIMEOUTS timeouts={0};
// timeouts.ReadIntervalTimeout=50;
// timeouts.ReadTotalTimeoutConstant=50;
// timeouts.ReadTotalTimeoutMultiplier=10;

// timeouts.WriteTotalTimeoutConstant=50;
// timeouts.WriteTotalTimeoutMultiplier=10;
// if(!SetCommTimeouts(comPort, &timeouts)){
// //error occureed. Inform user
// }

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
    if (data.rfind("AT", 0) != 0) { 
        throw std::runtime_error("Trying to send something that is not an AT command. To prevent self harm this is currently not possible");
        
    }
    // Convert the string to a vector of uint8_t
    std::vector<uint8_t> byte_data(data.begin(), data.end());

    // Call the `write` function with the converted data
    return write(byteData);
}

bool SerialWindows::write(const std::vector<uint8_t>& data) {
    if (comPort == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytes_written;
    return WriteFile(comPort, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, NULL);
}

std::string SerialWindows::read() {
    if (comPort == INVALID_HANDLE_VALUE) {
        return "";
    }
    std::vector<uint8_t> buffer(1024); // Example buffer size
    unsigned long bytes_read;
    ReadFile(comPort, buffer.data(), buffer.size(), &bytesRead, NULL);
    if(bytes_read > 0) {
        buffer.resize(bytes_read); // Adjust size to actual bytes read
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
    std::string string_buffer;
    while (running) {
        WaitCommEvent(comPort, &reason, &ov);
        if (WaitForSingleObject(ov.hEvent, INFINITE) == WAIT_OBJECT_0) {
            std::vector<uint8_t> buffer(1024); // Example buffer size
            unsigned long bytes_read;
            bool ret = ReadFile(comPort, buffer.data(), buffer.size(), &bytesRead, &ov);
            if(bytes_read > 0) {
                for(uint8_t character : buffer) {
                    if(character == '\r') {   
                        if(!(stringBuffer.empty())) { 
                            this->callbackPtr(std::move(stringBuffer));
                            stringBuffer.clear();
                        }
                    }
                    else if(character != 0) {
                        stringBuffer.push_back(character);
                    }
                }
            }
        }
    }

    CloseHandle(ov.hEvent);
}
