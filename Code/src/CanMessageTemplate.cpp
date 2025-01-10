#include <CanMessageTemplate.h>
#include <cstdint>

CanMessageTemplate::CanMessageTemplate(pugi::xml_node& msg) {
    name = msg.attribute("Name").as_string();
    description = msg.attribute("Description").as_string();   
  
    for (auto value = msg.child("Values").child("Value"); value; value = value.next_sibling("Value")) {
        value_list.emplace_back(value);
    }
}
std::vector<int64_t>
CanMessageTemplate::parseData(std::vector<uint8_t> can_data)
{
    std::vector<int64_t> values(this->value_list.size());
    uint16_t ii = 0;
    for(auto value : this->value_list) {
        std::vector<uint8_t> parsed_value = value.parseData(can_data);
        switch(parsed_value.size()){
            case sizeof(uint8_t):
                values[ii] = *((uint8_t*)parsed_value.data());
                break;
            case sizeof(uint16_t):
                values[ii] = *((uint16_t*)parsed_value.data());
                break;
            case sizeof(uint32_t):
                values[ii] = *((uint32_t*)parsed_value.data());
                break;
            case sizeof(uint64_t):
                values[ii] = *((uint64_t*)parsed_value.data());
                break;
            default:
                throw std::runtime_error("data must be exactly the size of a default integer type (1,2,4 or 8 bytes).");
            
        }
        ii++;
    }
    return values;
}
