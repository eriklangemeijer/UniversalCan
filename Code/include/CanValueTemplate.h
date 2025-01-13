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

        template<typename T> std::vector<uint8_t> applyBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift);
        template<typename T, typename Op> std::vector<uint8_t> applyOperation(std::vector<uint8_t> data, T B, Op operation);
        template<typename T> void copyTypeToData(T value, std::vector<uint8_t>& data);
        template<typename T> T convertDataToType(const std::vector<uint8_t>& data);
    public:
        
        CanValueTemplate(pugi::xml_node template_description);
        std::vector<uint8_t> parseData(std::vector<uint8_t> can_data);
        std::vector<uint8_t> modifierSelectByte(std::vector<uint8_t> data, uint8_t bit_start, uint8_t bit_end);
        template<typename Op> std::vector<uint8_t> callOperationForDatatype(std::vector<uint8_t> data, uint64_t B, Op operation);
        std::vector<uint8_t> modifierBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift);


};