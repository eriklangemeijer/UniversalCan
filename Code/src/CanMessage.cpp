#include <CanMessageTemplate.h>
#include <CanMessage.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

CanMessage::CanMessage(std::vector<uint8_t> data,
                       std::shared_ptr<CanMessageTemplate> msg_template,
                       uint8_t /*priority*/,
                       uint8_t /*receiver*/,
                       uint8_t /*transmitter*/)
  : data(data)
  , msg_template(msg_template)

{
    if(msg_template != nullptr) {
        values = msg_template->parseData(data);
    }
}

std::string CanMessage::to_string() {
    std::stringstream ss;
    ss << "CanMessage:\n";

    if(this->msg_template == nullptr) {
        ss << ("\tType:UNKNOWN\n" );
        ss << ("\tDATA: {" );
        for(size_t ii = 0; ii < data.size(); ++ii) {
            ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[ii]);
            if(ii == data.size() - 1) {
                ss << "}\n";
            }
            else {
                ss << ", ";
            }
        }
    }
    else {
        ss << ("\tType:" + msg_template->getName() + "\n" );
        ss << ("\tValues:\n" );
        for(auto value : this->values) {
            ss << ("\t\t" + value.getValueName() + ":");
            std::string const data_type = value.getDataType();
            if(data_type == "bool") {
                ss << (value.getBoolValue() ? "true" : "false");
            }
            else if(data_type == "uint8") {
                ss << (std::to_string(value.getValue<uint8_t>()) + "\n");
            }
            else if(data_type == "uint16") {
                ss << (std::to_string(value.getValue<uint16_t>()) + "\n");
            }
            else if(data_type == "uint32") {
                ss << (std::to_string(value.getValue<uint32_t>()) + "\n");
            }
            else if(data_type == "uint64") {
                ss << (std::to_string(value.getValue<uint64_t>()) + "\n");
            }
            else {
                ss << ("Unknown data type" + data_type + "\n");
            }
        }
    }
    return ss.str();
}

void CanMessage::print() {
    std::cout << this->to_string();
}
