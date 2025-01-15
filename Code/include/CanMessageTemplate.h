#pragma once

#include <stdint.h>
#include <vector>
#include <functional>
#include <string>
#include <list>
#include <pugixml.hpp>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <CanValue.h>

class CanMessageTemplate
{
    private:
        std::string name;
        std::string description;
        std::list<CanValueTemplate> value_list;
        std::shared_ptr<ModifierFunction> filter_function;
    public:
        
        CanMessageTemplate(pugi::xml_node& msg);
        std::vector<CanValue> parseData(std::vector<uint8_t> can_data);
        bool isMatch(std::vector<uint8_t> can_data);
        std::string getName();



};