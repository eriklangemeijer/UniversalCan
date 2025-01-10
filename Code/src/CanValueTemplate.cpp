#include <CanValueTemplate.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <vector>

const uint8_t max_value_size = 8;

CanValueTemplate::CanValueTemplate(pugi::xml_node template_description)
{
    value_name = template_description.attribute("name").as_string();
    data_type = template_description.attribute("DataType").as_string();

    auto operations = template_description.child("Operations");
    for (auto operation = operations.first_child(); operation; operation = operation.next_sibling()) {
        if(strcmp(operation.name(), "BYTE_SELECT") == 0 ) {
            uint64_t arg1 = operation.attribute("arg1").as_int();
            uint64_t arg2 = operation.attribute("arg2").as_int();
            func_list.emplace_back(
              [this, arg1, arg2](std::vector<uint8_t> data) {
                return modifierSelectByte(data, arg1, arg2);
              });
        }
        else if(strcmp(operation.name(), "GAIN") == 0 ) {
            uint64_t arg1 = operation.attribute("arg1").as_int();
            func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
              return modifierGain(data, arg1);
            });
        }
        else if(strcmp(operation.name(), "BITWISE_OR") == 0 ) {
            uint64_t arg1 = operation.attribute("arg1").as_int();
            func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
              return modifierBitwiseOr(data, arg1);
            });
        }
        else if(strcmp(operation.name(), "BITWISE_AND") == 0 ) {
            uint64_t arg1 = operation.attribute("arg1").as_int();
            func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
              return modifierBitwiseAnd(data, arg1);
            });
        }
        else if(strcmp(operation.name(), "BITSHIFT_RIGHT") == 0 ) {
            uint64_t arg1 = operation.attribute("arg1").as_int();
            func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
              return modifierBitShiftRight(data, arg1);
            });
        }
        else if(strcmp(operation.name(), "BITSHIFT_LEFT") == 0 ) {
            uint64_t arg1 = operation.attribute("arg1").as_int();
            func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
              return modifierBitShiftLeft(data, arg1);
            });
        }
        else {
            std::cout<< "Unsupported function: " << operation.name() << std::endl;
        }
    }
  
}

std::vector<uint8_t>
CanValueTemplate::parseData(std::vector<uint8_t> can_data)
{
  for(auto modifier_function : this->func_list) {
    can_data = modifier_function(can_data);    
  }  
  return can_data;
}

std::vector<uint8_t>
CanValueTemplate::modifierSelectByte(std::vector<uint8_t> data,
                                   uint8_t byte_start,
                                   uint8_t byte_end)
{
    if(byte_start > byte_end || (byte_end-byte_start) > max_value_size) {
        throw std::runtime_error("byte_start must be smaller than byte_end");
    }
    std::vector<uint8_t> ret_data;
    ret_data.assign(data.begin()+byte_start, data.begin()+byte_end);
    return ret_data;
}

std::vector<uint8_t>
CanValueTemplate::modifierGain(std::vector<uint8_t> data, uint64_t B)
{
    switch(data.size()){
        case sizeof(uint8_t):
            B *= *((uint8_t*)data.data());
            break;
        case sizeof(uint16_t):
            B *= *((uint16_t*)data.data());
            break;
        case sizeof(uint32_t):
            B *= *((uint32_t*)data.data());
            break;
        case sizeof(uint64_t):
            B *= *((uint64_t*)data.data());
            break;
        default:
            throw std::runtime_error("data must be exactly the size of a default integer type (1,2,4 or 8 bytes)");
        
    }
    std::memcpy(data.data(), &B, data.size());
    return data;
}

std::vector<uint8_t>
CanValueTemplate::modifierBitwiseOr(std::vector<uint8_t> data, uint64_t B) {
    for(uint8_t ii=0; ii < data.size(); ii++) {
        data[ii] |= *(((uint8_t*)&B));
    }
    return data;
}

std::vector<uint8_t>
CanValueTemplate::modifierBitwiseAnd(std::vector<uint8_t> data, uint64_t B) {
    for(uint8_t ii=0; ii < data.size(); ii++) {
        data[ii] &= *(((uint8_t*)&B));
    }
    return data;
}


std::vector<uint8_t> CanValueTemplate::modifierBitShiftRight(std::vector<uint8_t> data, uint64_t nr_bits) {
    uint64_t temp_data;
    switch(data.size()){
        case sizeof(uint8_t):
            temp_data = *((uint8_t*)data.data());
            break;
        case sizeof(uint16_t):
            temp_data = *((uint16_t*)data.data());
            break;
        case sizeof(uint32_t):
            temp_data = *((uint32_t*)data.data());
            break;
        case sizeof(uint64_t):
            temp_data = *((uint64_t*)data.data());
            break;
        default:
            throw std::runtime_error("data must be exactly the size of a default integer type (1,2,4 or 8 bytes)");
        
    }
    temp_data >>= nr_bits;
    std::memcpy(data.data(), &temp_data, data.size());
    return data;
}

std::vector<uint8_t> CanValueTemplate::modifierBitShiftLeft(std::vector<uint8_t> data, uint64_t nr_bits) {
    uint64_t temp_data;
    switch(data.size()){
        case sizeof(uint8_t):
            temp_data = *((uint8_t*)data.data());
            break;
        case sizeof(uint16_t):
            temp_data = *((uint16_t*)data.data());
            break;
        case sizeof(uint32_t):
            temp_data = *((uint32_t*)data.data());
            break;
        case sizeof(uint64_t):
            temp_data = *((uint64_t*)data.data());
            break;
        default:
            throw std::runtime_error("data must be exactly the size of a default integer type (1,2,4 or 8 bytes)");
        
    }
    temp_data <<= nr_bits;
    std::memcpy(data.data(), &temp_data, data.size());
    return data;
}