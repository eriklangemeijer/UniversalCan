#include "CanMessageTemplate.h"
#include <CanMessage.h>
#include <cstdint>
#include <memory>
#include <vector>

CanMessage::CanMessage(std::vector<uint8_t> data,
                       std::shared_ptr<CanMessageTemplate> msg_template,
                       uint8_t priority,
                       uint8_t receiver,
                       uint8_t transmitter) : data(data), msg_template(msg_template)

{
    values = msg_template->parseData(data);
}
