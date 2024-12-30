#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "ISerial.h"
#include "CanMessage.h"

class ELM327 {
private:
    std::unique_ptr<ISerial> serial; // Serial interface for communication
    bool running; // Flag to indicate if the callback thread is running
    std::function<void(CanMessage)> callbackFunction; // User-defined callback for received CAN messages

public:
    // Constructor accepting a unique_ptr to a serial implementation
    explicit ELM327(std::unique_ptr<ISerial> serial);

    // Destructor
    ~ELM327();

    // Send a list of CAN messages
    bool sendMessage(std::vector<CanMessage> messages);

    // Register a callback function for incoming CAN messages
    void registerCallback(std::function<void(CanMessage)> callback);
};
