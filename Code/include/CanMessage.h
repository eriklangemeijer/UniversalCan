#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <CanMessageTemplate.h>
class CanMessage
{
    public:
        CanMessage(std::vector<uint8_t> data, std::unique_ptr<CanMessageTemplate> msg_template);
        uint8_t priority;
        uint8_t receiver;
        uint8_t transmitter;
        uint8_t Data[7];
        uint8_t checksum;
        std::unique_ptr<CanMessageTemplate> msg_template; 

};