#include <ModifierFunction.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include <limits>
const uint8_t max_value_size = 8;
const uint8_t nr_of_arguments = 2;
ModifierFunction::ModifierFunction(pugi::xml_attribute attribute) {
    int const const_value = attribute.as_int();
    function = ([this, const_value](std::vector<uint8_t>, std::vector<ModifierFunction>) {    
        
        auto value = copyTypeToData(const_value);
        return value;
    });
}

ModifierFunction::ModifierFunction(pugi::xml_node operation)
    : name(operation.name()) {

    for (auto argument = operation.first_child(); argument != nullptr; argument = argument.next_sibling()) {
        arguments.emplace_back(argument);
    }

    std::regex const pattern(R"(arg(\d+))");
    for (pugi::xml_attribute const attr : operation.attributes()) {
        std::smatch match;
        std::string const attr_name = attr.name();
        if (std::regex_match(attr_name, match, pattern)) {
            int const index = std::stoi(match[1].str());
            if (index >= 1 && index <= nr_of_arguments) {
                arguments.insert(arguments.begin() + index - 1, attr);
            } else {
                throw std::runtime_error("Invalid argument number " + std::to_string(index));
            }
        }
    }

    if (strcmp(operation.name(), "BYTE_SELECT") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return modifierSelectByte(data, args);
            });
    } else if (strcmp(operation.name(), "BITWISE_OR") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::bit_or<>());
            });
    } else if (strcmp(operation.name(), "BITWISE_AND") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::bit_and<>());
            });
    } else if (strcmp(operation.name(), "BITSHIFT_RIGHT") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return modifierBitShift(data, args, false);
            });
    } else if (strcmp(operation.name(), "BITSHIFT_LEFT") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return modifierBitShift(data, args, true);
            });
    } else if (strcmp(operation.name(), "INT_COMPARE") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::equal_to<>());
            });
    } else if (strcmp(operation.name(), "LOGICAL_AND") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::logical_and<>());
            });
    } else if (strcmp(operation.name(), "MULTIPLY") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::multiplies<>());
            });
    } else if (strcmp(operation.name(), "DIVIDE") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::divides<>());
            });
    } else if (strcmp(operation.name(), "ADD") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::plus<>());
            });
    } else if (strcmp(operation.name(), "SUBTRACT") == 0) {
        function =
            ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
                return callOperationForDatatype(data, args, std::minus<>());
            });
    } else if (strcmp(operation.name(), "CONSTANT") == 0) {
        int const const_value = operation.attribute("value").as_int();
        function = ([this, const_value](std::vector<uint8_t>,
                                        std::vector<ModifierFunction>) {
            auto value = copyTypeToData(const_value);
            return value;
        });

    } else {
        throw std::runtime_error("Unsupported function " + std::string(operation.name()) + "\n");
    }
}

std::vector<uint8_t> ModifierFunction::modifierSelectByte(std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
    if (args.size() != nr_of_arguments) {
        throw std::runtime_error("Incorrect number of arguments provided");
    }
    auto  byte_start = convertDataToType<uint16_t>(args[0].call(data));
    auto  byte_end = convertDataToType<uint16_t>(args[1].call(data));
    if (byte_start >= byte_end) {
        throw std::runtime_error("byte_start must be smaller than byte_end");
    }
    if ((byte_end - byte_start) > max_value_size) {
        throw std::runtime_error("Cannot select more than 8 bytes");
    }
    if ((byte_end) > data.size()) {
        throw std::runtime_error("Selection is out of range");
    }
    std::vector<uint8_t> ret = {data.begin() + byte_start, data.begin() + byte_end};
    return ret;
}

template <typename T, typename Op>
std::vector<uint8_t> ModifierFunction::applyOperation(std::vector<uint8_t> data, T argument1, Op operation) {
    //Converting lhs value to largest integer to prevent overflow
    uint64_t value = convertDataToType<uint64_t>(data);
    uint64_t output = operation(value, argument1);
    //Converting back to minimal integer size to reduce padding
    return copyTypeToData(output);
}

template <typename Op>
std::vector<uint8_t> ModifierFunction::callOperationForDatatype(std::vector<uint8_t> data, std::vector<ModifierFunction> args, Op operation) {
    if (args.size() != 2) {
        throw std::runtime_error("Incorrect number of arguments provided");
    }
    std::vector<uint8_t> const &input_value = args[0].call(data);
    auto const argument1 = convertDataToType<uint16_t>(args[1].call(data));
    switch (input_value.size()) {
    case sizeof(uint8_t):
        return applyOperation<uint8_t>(input_value, (uint8_t)argument1, operation);
    case sizeof(uint16_t):
        return applyOperation<uint16_t>(input_value, (uint16_t)argument1, operation);
    case sizeof(uint32_t):
        return applyOperation<uint32_t>(input_value, (uint32_t)argument1, operation);
    case sizeof(uint64_t):
        return applyOperation<uint64_t>(input_value, (uint64_t)argument1, operation);
    default:
        throw std::runtime_error("data must be exactly the size of a default "
                                 "integer type (1,2,4 or 8 bytes)");
    }
}

template <typename T>
std::vector<uint8_t> ModifierFunction::applyBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool is_left_shift) {
    T value = convertDataToType<T>(data);
    if (is_left_shift) {
        value <<= nr_bits;
    } else {
        value >>= nr_bits;
    }
    return copyTypeToData(value);
}

std::vector<uint8_t> ModifierFunction::modifierBitShift(std::vector<uint8_t> data, std::vector<ModifierFunction> args, bool is_left_shift) {
    if (args.size() < 2) {
        throw std::runtime_error("modifierBitShift requires at least 2 arguments");
    }

    std::vector<uint8_t> const value_to_shift = args[0].call(data);
    auto const nr_bits = convertDataToType<uint16_t>(args[1].call(data));
    switch (value_to_shift.size()) {
    case sizeof(uint8_t):
        return applyBitShift<uint8_t>(value_to_shift, nr_bits, is_left_shift);
    case sizeof(uint16_t):
        return applyBitShift<uint16_t>(value_to_shift, nr_bits, is_left_shift);
    case sizeof(uint32_t):
        return applyBitShift<uint32_t>(value_to_shift, nr_bits, is_left_shift);
    case sizeof(uint64_t):
        return applyBitShift<uint64_t>(value_to_shift, nr_bits, is_left_shift);
    default:
        throw std::runtime_error("data must be exactly the size of a default "
                                 "integer type (1,2,4 or 8 bytes)");
    }
}

template <typename T>
T ModifierFunction::convertDataToType(const std::vector<uint8_t> &data) {
    switch (data.size()) {
        case sizeof(uint8_t):
            return *((uint8_t*)(data.data()));
        case sizeof(uint16_t):
            return *((uint16_t*)(data.data()));
        case sizeof(uint32_t):
            return *((uint32_t*)(data.data()));
        case sizeof(uint64_t):
            return *((uint64_t*)(data.data()));
    default:
        throw std::runtime_error("data must be exactly the size of a default "
                                 "integer type (1,2,4 or 8 bytes)");
    }
}

template <typename T>
std::vector<uint8_t> ModifierFunction::copyTypeToData(T value) {
    uint8_t value_size = sizeof(value);
    if(value <= std::numeric_limits<uint8_t>::max()) {
        value_size = sizeof(uint8_t);
    } else if(value <= std::numeric_limits<uint16_t>::max()) {
        value_size = sizeof(uint16_t);
    } else if(value <= std::numeric_limits<uint32_t>::max()) {
        value_size = sizeof(uint32_t);
    } else {
        value_size = sizeof(uint64_t);
    }
    std::vector<uint8_t> data = std::vector<uint8_t>(value_size);
    std::memcpy(data.data(), &value, data.size());
    return data;
}

std::vector<uint8_t> ModifierFunction::call(std::vector<uint8_t> can_data) {
    if (this->function) {
    return this->function(can_data, this->arguments);
    }
    throw std::runtime_error("Function is not initialized");
}