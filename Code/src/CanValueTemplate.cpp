#include <CanValueTemplate.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>

const uint8_t max_value_size = 8;

CanValueTemplate::CanValueTemplate(pugi::xml_node template_description)
{
    value_name = template_description.attribute("name").as_string();
    data_type = template_description.attribute("DataType").as_string();

    auto operations = template_description.child("Operations");
    for (auto operation = operations.first_child(); operation; operation = operation.next_sibling()) {
        if(strcmp(operation.name(), "BYTE_SELECT") == 0 ) {
          uint64_t const arg1 = operation.attribute("arg1").as_int();
          uint64_t const arg2 = operation.attribute("arg2").as_int();
          func_list.emplace_back([this, arg1, arg2](std::vector<uint8_t> data) {
            return modifierSelectByte(data, arg1, arg2);
          });
        }
        else if(strcmp(operation.name(), "GAIN") == 0 ) {
          uint64_t const arg1 = operation.attribute("arg1").as_int();
          func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
            return callOperationForDatatype(data, arg1, std::multiplies<>());
          });
        }
        else if(strcmp(operation.name(), "BITWISE_OR") == 0 ) {
          uint64_t const arg1 = operation.attribute("arg1").as_int();
          func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
            return callOperationForDatatype(data, arg1, std::bit_or<>());
          });
        }
        else if(strcmp(operation.name(), "BITWISE_AND") == 0 ) {
          uint64_t const arg1 = operation.attribute("arg1").as_int();
          func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
            return callOperationForDatatype(data, arg1, std::bit_and<>());
          });
        }
        else if(strcmp(operation.name(), "BITSHIFT_RIGHT") == 0 ) {
          uint64_t const arg1 = operation.attribute("arg1").as_int();
          func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
            return modifierBitShift(data, arg1, false);
          });
        }
        else if(strcmp(operation.name(), "BITSHIFT_LEFT") == 0 ) {
          uint64_t const arg1 = operation.attribute("arg1").as_int();
          func_list.emplace_back([this, arg1](std::vector<uint8_t> data) {
            return modifierBitShift(data, arg1, true);
          });
        }
        else {
            std::cout << "Unsupported function: " << operation.name() << std::endl;
        }
    }
}




std::vector<uint8_t> CanValueTemplate::parseData(std::vector<uint8_t> can_data)
{
    for(auto modifier_function : this->func_list) {
        can_data = modifier_function(can_data);    
    }  
    return can_data;
}

std::vector<uint8_t> CanValueTemplate::modifierSelectByte(std::vector<uint8_t> data, uint8_t byte_start, uint8_t byte_end)
{
    if(byte_start > byte_end || (byte_end - byte_start) > max_value_size) {
        throw std::runtime_error("byte_start must be smaller than byte_end");
    }
    return { data.begin() + byte_start, data.begin() + byte_end };
}



template<typename T, typename Op>
std::vector<uint8_t> CanValueTemplate::applyOperation(std::vector<uint8_t> data, T B, Op operation) {
    T value = convertDataToType<T>(data);
    value = operation(value, static_cast<T>(B));  // Apply the bitwise operation using the passed operation
    copyTypeToData(value, data);
    return data;
}

template<typename Op>
std::vector<uint8_t> CanValueTemplate::callOperationForDatatype(std::vector<uint8_t> data, uint64_t B, Op operation)
{
    switch (data.size()) {
        case sizeof(uint8_t):
            return applyOperation<uint8_t>(data, (uint8_t)B, operation);
        case sizeof(uint16_t):
            return applyOperation<uint16_t>(data, (uint16_t)B, operation);
        case sizeof(uint32_t):
            return applyOperation<uint32_t>(data, (uint32_t)B, operation);
        case sizeof(uint64_t):
            return applyOperation<uint64_t>(data, (uint64_t)B, operation);
        default:
            throw std::runtime_error("data must be exactly the size of a default integer type (1,2,4 or 8 bytes)");
    }
}

template<typename T>
std::vector<uint8_t> CanValueTemplate::applyBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift) {
    T value = convertDataToType<T>(data);
    if (isLeftShift) {
        value <<= nr_bits;
    } else {
        value >>= nr_bits;
    }
    copyTypeToData(value, data);
    return data;
}

std::vector<uint8_t> CanValueTemplate::modifierBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift)
{
    switch (data.size()) {
        case sizeof(uint8_t):
            return applyBitShift<uint8_t>(data, nr_bits, isLeftShift);
        case sizeof(uint16_t):
            return applyBitShift<uint16_t>(data, nr_bits, isLeftShift);
        case sizeof(uint32_t):
            return applyBitShift<uint32_t>(data, nr_bits, isLeftShift);
        case sizeof(uint64_t):
            return applyBitShift<uint64_t>(data, nr_bits, isLeftShift);
        default:
            throw std::runtime_error("data must be exactly the size of a default integer type (1,2,4 or 8 bytes)");
    }
}

template<typename T>
T CanValueTemplate::convertDataToType(const std::vector<uint8_t>& data) {
    if (data.size() != sizeof(T)) {
        throw std::runtime_error("Data size mismatch");
    }
    T value;
    std::memcpy(&value, data.data(), sizeof(T));
    return value;
}

template<typename T>
void CanValueTemplate::copyTypeToData(T value, std::vector<uint8_t>& data) {
    std::memcpy(data.data(), &value, sizeof(T));
}
