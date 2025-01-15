#include "CanMessageTemplate.h"
#include <CanMessage.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

CanMessage::CanMessage(std::vector<uint8_t> data,
                       std::shared_ptr<CanMessageTemplate> msg_template,
                       uint8_t /*priority*/,
                       uint8_t /*receiver*/,
                       uint8_t /*transmitter*/)
  : data(data)
  , msg_template(msg_template)

{
    values = msg_template->parseData(data);
}

std::string CanMessage::to_string() {
    std::string output_string = "Canmessage:\n";
    if(this->msg_template == nullptr) {
        output_string.append("\tType:UNKNOWN\n" );
    }
    else {
        output_string.append("\tType:" + msg_template->getName() + "\n" );
    }
        output_string.append("\tValues:\n" );
    for(auto value : this->values) {
        output_string.append("\t\t" + value.getValueName() + ":");
        std::string data_type = value.getDataType();
        if(data_type == "bool") {
            output_string.append(value.getBoolValue() ? "true" : "false");
        }
        else if(data_type == "uint8") {
            output_string.append(std::to_string(value.getValue<uint8_t>()) + "\n");
        }
        else if(data_type == "uint16") {
            output_string.append(std::to_string(value.getValue<uint16_t>()) + "\n");
        }
        else if(data_type == "uint32") {
            output_string.append(std::to_string(value.getValue<uint32_t>()) + "\n");
        }
        else if(data_type == "uint64") {
            output_string.append(std::to_string(value.getValue<uint64_t>()) + "\n");
        }
        else {
            output_string.append("Unknown data type" + data_type + "\n");
        }
    }
    return output_string;
}

void CanMessage::print() {
    std::cout << this->to_string();
}
