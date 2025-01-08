#pragma once

#include <stdint.h>
#include <vector>
class CanMessage
{
    public:
        CanMessage(std::vector<uint8_t> data);
        uint8_t priority;
        uint8_t receiver;
        uint8_t transmitter;
        uint8_t Data[7];
        uint8_t checksum;

};