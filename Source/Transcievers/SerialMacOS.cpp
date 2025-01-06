#include "SerialMacOS.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <chrono>

SerialMacOS::SerialMacOS() : fd_(-1), running_(false) {}

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

    running_ = true;
    readerThread_ = std::thread(&SerialMacOS::readLoop, this);

    return true;
}

void SerialMacOS::close() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (running_) {
        running_ = false;
        if (readerThread_.joinable()) {
            readerThread_.join();
        }
    }

    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
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

bool SerialMacOS::read(std::vector<uint8_t>& buffer, size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (fd_ == -1) {
        std::cerr << "Port not open for reading." << std::endl;
        return false;
    }

    buffer.resize(size);
    ssize_t bytesRead = ::read(fd_, buffer.data(), size);
    if (bytesRead <= 0) {
        buffer.clear();
        return false;
    }

    buffer.resize(bytesRead);
    return true;
}

void SerialMacOS::registerCallback(std::function<void(std::vector<uint8_t>)> callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callback_ = callback;
}

void SerialMacOS::readLoop() {
    std::vector<uint8_t> buffer(1024);

    while (running_) {
        ssize_t bytesRead = ::read(fd_, buffer.data(), buffer.size());
        if (bytesRead > 0) {
            std::vector<uint8_t> data(buffer.begin(), buffer.begin() + bytesRead);
            std::lock_guard<std::mutex> lock(callbackMutex_);
            if (callback_) {
                callback_(data);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
