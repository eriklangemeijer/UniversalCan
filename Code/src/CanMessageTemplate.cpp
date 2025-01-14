#include "CanValueTemplate.h"
#include "ModifierFunction.h"
#include <CanMessageTemplate.h>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

CanMessageTemplate::CanMessageTemplate(pugi::xml_node& msg) {
    name = msg.attribute("Name").as_string();
    description = msg.attribute("Description").as_string();   
  
    for (auto value = msg.child("Values").child("Value"); value; value = value.next_sibling("Value")) {
        value_list.emplace_back(value);
    }

    
    filter_function = std::make_shared<ModifierFunction>(msg.child("FILTER_FUNCTION").first_child());

}

bool CanMessageTemplate::isMatch(std::vector<uint8_t> can_data)
{   
    std::vector<uint8_t> result = this->filter_function->call(can_data);
    if(result.size() == 1) {
        return (result.data()[0] == 1);
    }
    throw std::runtime_error("not size bool");
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

std::string CanMessageTemplate::getName() {
    return this->name;
}
