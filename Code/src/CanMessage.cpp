#include <CanMessageTemplate.h>
#include <CanMessage.h>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

CanMessage::CanMessage(std::vector<uint8_t> data,
                       std::shared_ptr<CanMessageTemplate> msg_template,
                       uint8_t /*priority*/,
                       uint8_t /*receiver*/,
                       uint8_t /*transmitter*/)
    : data(data), msg_template(msg_template)

{
    if (msg_template != nullptr) {
        values = msg_template->parseData(data);
    }
}

std::string CanMessage::to_string() {
    std::stringstream string_stream;
    string_stream << "<CanMessage ";

    if (this->msg_template == nullptr) {
        string_stream << ("Type=\"UNKNOWN\">\n");
        string_stream << ("\t<Values data=\"{");
        for (size_t ii = 0; ii < data.size(); ++ii) {
            string_stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[ii]);
            if (ii == data.size() - 1) {
                string_stream << "}\"/>\n";
            } else {
                string_stream << ", ";
            }
        }
    } else {
        string_stream << ("Type=\"" + msg_template->getName() + "\">\n");
        string_stream << ("\t<Values>\n");
        for (auto value : this->values) {
            string_stream << ("\t\t<Value name=\"" + value.getValueName() + "\" datatype=\"" + value.getDataType() + "\" value=\"");
            std::string const data_type = value.getDataType();
            if (data_type == "bool") {
                string_stream << (value.getBoolValue() ? "true" : "false");
            } else if (data_type == "uint8") {
                string_stream << (std::to_string(value.getValue<uint8_t>()));
            } else if (data_type == "uint16") {
                string_stream << (std::to_string(value.getValue<uint16_t>()));
            } else if (data_type == "uint32") {
                string_stream << (std::to_string(value.getValue<uint32_t>()));
            } else if (data_type == "uint64") {
                string_stream << (std::to_string(value.getValue<uint64_t>()));
            } else {
                string_stream << ("Unknown data type \"" + data_type + "\"\n");
            }
            string_stream << ("\"/>\n");
        }
        string_stream << ("\t</Values>\n");
    }
    string_stream << "</CanMessage>\n";
    return string_stream.str();
}

void CanMessage::print() {
    std::cout << this->to_string();
}
