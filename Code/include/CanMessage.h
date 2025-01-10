#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <CanMessageTemplate.h>
class CanMessage
{
    public:
        CanMessage(std::vector<uint8_t> data, std::shared_ptr<CanMessageTemplate> msg_template, uint8_t priority=0, uint8_t receiver=0, uint8_t transmitter=0);
        std::vector<uint8_t> data;
        uint8_t priority;
        uint8_t receiver;
        uint8_t transmitter;
        std::shared_ptr<CanMessageTemplate> msg_template;
        std::vector<int64_t> values;
        

};