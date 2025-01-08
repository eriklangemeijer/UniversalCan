#include "SerialMacOS.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <chrono>

SerialMacOS::SerialMacOS() : fd_(-1), running(false) {}

SerialMacOS::~SerialMacOS() {
    close();
}

bool SerialMacOS::open(const std::string& port) {
    std::lock_guard<std::mutex> lock(mutex_);

    fd_ = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ == -1) {
        std::cerr << "Failed to open port: " << port << std::endl;
        return false;
    }

    struct termios options;
    tcgetattr(fd_, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd_, TCSANOW, &options);

    this->running = true;

    return true;
}

void SerialMacOS::close() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (this->running) {
        this->running = false;
    }

    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool SerialMacOS::writeString(std::string data) {
    if (data.rfind("AT", 0) != 0) { 
        throw std::runtime_error("Trying to send something that is not an AT command. To prevent self harm this is currently not possible");
        
    }
    // Convert the string to a vector of uint8_t
    std::vector<uint8_t> byteData(data.begin(), data.end());

    // Call the `write` function with the converted data
    return write(byteData);
}

bool SerialMacOS::write(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (fd_ == -1) {
        std::cerr << "Port not open for writing." << std::endl;
        return false;
    }

    ssize_t bytesWritten = ::write(fd_, data.data(), data.size());
    return bytesWritten == static_cast<ssize_t>(data.size());
}

std::string SerialMacOS::read() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (fd_ == -1) {
        std::cerr << "Port not open for reading." << std::endl;
        return "";
    }
    std::vector<uint8_t> buffer(1024); // Example buffer size
    ssize_t bytesRead = ::read(fd_, buffer.data(), buffer.size());
    if (bytesRead <= 0) {
        buffer.clear();
        return "";
    }

    buffer.resize(bytesRead);
    std::string stringBuffer(buffer.begin(), buffer.end());
    return stringBuffer;
}

void SerialMacOS::registerCallback(std::function<void(std::string)> callback) {
    this->callbackPtr = callback;
    running = true;
    std::thread(&SerialMacOS::threadFunction, this).detach();
}


void SerialMacOS::threadFunction() {
    std::vector<uint8_t> buffer(1024);

    while (this->running) {
        ssize_t bytesRead = ::read(fd_, buffer.data(), buffer.size());
        if (bytesRead > 0) {
            std::string stringBuffer(buffer.begin(), buffer.begin() + bytesRead);
            std::lock_guard<std::mutex> lock(callbackMutex_);
            if (this->callbackPtr) {
                this->callbackPtr(stringBuffer);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
