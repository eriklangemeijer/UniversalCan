#include "CanValue.h"
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


std::vector<CanValue>
CanMessageTemplate::parseData(std::vector<uint8_t> can_data)
{
    std::vector<CanValue> values;
    for(auto value : this->value_list) {
      values.emplace_back(value, can_data);
    }
    return values;
}

std::string CanMessageTemplate::getName() {
    return this->name;
}
