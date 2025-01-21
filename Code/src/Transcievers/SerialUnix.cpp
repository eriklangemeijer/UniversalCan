#include "Transcievers/ISerial.h"
#include <Transcievers/SerialUnix.h>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <sys/_types/_ssize_t.h>
#include <sys/fcntl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <vector>

SerialUnix::SerialUnix() : fd_(-1), running(false) {}

SerialUnix::~SerialUnix() {
    close();
}

bool SerialUnix::open(const std::string &port) {
    std::lock_guard<std::mutex> const lock(mutex_);

    fd_ = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ == -1) {
        std::cerr << "Failed to open port: " << port << std::endl;
        return false;
    }

    struct termios options{};
    tcgetattr(fd_, &options);
    cfsetispeed(&options, B38400);
    cfsetospeed(&options, B38400);
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

void SerialUnix::close() {
    std::lock_guard<std::mutex> const lock(mutex_);

    if (this->running) {
        this->running = false;
    }

    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool SerialUnix::writeString(std::string data) {
    // if (data.rfind("AT", 0) != 0) {
    //     throw std::runtime_error("Trying to send something that is not an AT command. To prevent self harm this is currently not possible");
    // }
    // Convert the string to a vector of uint8_t
    std::vector<uint8_t> const byte_data(data.begin(), data.end());

    // Call the `write` function with the converted data
    return write(byte_data);
}

bool SerialUnix::write(const std::vector<uint8_t> &data) {
    std::lock_guard<std::mutex> const lock(mutex_);

    if (fd_ == -1) {
        std::cerr << "Port not open for writing." << std::endl;
        return false;
    }

    ssize_t const bytes_written = ::write(fd_, data.data(), data.size());
    return bytes_written == static_cast<ssize_t>(data.size());
}

std::string SerialUnix::read() {
    throw std::runtime_error("Not implemented");
    // std::lock_guard<std::mutex> const lock(mutex_);

    // if (fd_ == -1) {
    //     std::cerr << "Port not open for reading." << std::endl;
    //     return "";
    // }
    // std::vector<uint8_t> buffer(SERIAL_READ_BUFFER_SIZE); // Example buffer size
    // ssize_t const bytes_read = ::read(fd_, buffer.data(), buffer.size());
    // if (bytes_read <= 0) {
    //     buffer.clear();
    //     return "";
    // }

    // buffer.resize(bytes_read);
    // std::string string_buffer(buffer.begin(), buffer.end());
    // return string_buffer;
}

void SerialUnix::registerCallback(std::function<void(std::vector<uint8_t>)> callback) {
    this->callbackPtr = callback;
    running = true;
    std::thread(&SerialUnix::threadFunction, this).detach();
}

void SerialUnix::threadFunction() {
    std::vector<uint8_t> rolling_buffer;

    while (this->running) {
        std::vector<uint8_t> read_buffer(SERIAL_READ_BUFFER_SIZE);
        ssize_t const bytes_read =
            ::read(fd_, read_buffer.data(), read_buffer.size());
        if (bytes_read > 0) {
            for (uint8_t const character : read_buffer) {
                if (character == '\r' || (!rolling_buffer.empty() && rolling_buffer.back() == '>')) {
                    if (!(rolling_buffer.empty())) {
                        this->callbackPtr(rolling_buffer);
                        rolling_buffer.clear();
                    }
                } else if (character != 0) {
                    rolling_buffer.push_back(character);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}