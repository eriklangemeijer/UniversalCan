#pragma once

#include <stdint.h>
#include <vector>
#include <functional>
#include <string>
#include <list>
#include <pugixml.hpp>
class CanValueTemplate
{
    private:
        std::string value_name;
        std::string data_type ;
        std::list<std::function<std::vector<uint8_t>(std::vector<uint8_t> data)>> func_list;
    public:
        
        CanValueTemplate(pugi::xml_node template_description);
        std::vector<uint8_t> parseData(std::vector<uint8_t> can_data);
        std::vector<uint8_t> modifierSelectByte(std::vector<uint8_t> data, uint8_t bit_start, uint8_t bit_end);
        std::vector<uint8_t> modifierGain(std::vector<uint8_t> , int64_t B);
        std::vector<uint8_t> modifierBitwiseOr(std::vector<uint8_t> data, int64_t B);
        std::vector<uint8_t> modifierBitwiseAnd(std::vector<uint8_t> data, int64_t B);



};