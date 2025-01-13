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

        template<typename T> std::vector<uint8_t> applyGain(std::vector<uint8_t> data, uint64_t B);
        template<typename T> std::vector<uint8_t> applyBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift);
        template<typename T, typename Op> std::vector<uint8_t> applyBitwiseOperation(std::vector<uint8_t> data, T B, Op op);
    public:
        
        CanValueTemplate(pugi::xml_node template_description);
        std::vector<uint8_t> parseData(std::vector<uint8_t> can_data);
        std::vector<uint8_t> modifierSelectByte(std::vector<uint8_t> data, uint8_t bit_start, uint8_t bit_end);
        std::vector<uint8_t> modifierGain(std::vector<uint8_t> , uint64_t B);
        template<typename Op> std::vector<uint8_t> modifierBitwise(std::vector<uint8_t> data, uint64_t B, Op op);
        std::vector<uint8_t> modifierBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift);



};