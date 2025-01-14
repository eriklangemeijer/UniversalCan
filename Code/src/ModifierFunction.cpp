#include <ModifierFunction.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <vector>
#include <iostream>

const uint8_t max_value_size = 8;

ModifierFunction::ModifierFunction(pugi::xml_node operation)
  : name(operation.name())
{

  for (auto value = operation.first_child(); value;
    value = value.next_sibling()) {
    arguments.emplace_back(value);
  }
  if (strcmp(operation.name(), "BYTE_SELECT") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return modifierSelectByte(data, args);
      });
  } 
  else if (strcmp(operation.name(), "GAIN") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return callOperationForDatatype(data, args, std::multiplies<>());
      });
  } 
  else if (strcmp(operation.name(), "BITWISE_OR") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return callOperationForDatatype(data, args, std::bit_or<>());
      });
  } 
  else if (strcmp(operation.name(), "BITWISE_AND") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return callOperationForDatatype(data, args, std::bit_and<>());
      });
  } 
  else if (strcmp(operation.name(), "BITSHIFT_RIGHT") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return modifierBitShift(data, args, false);
      });
  } 
  else if (strcmp(operation.name(), "BITSHIFT_LEFT") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return modifierBitShift(data, args, true);
      });
  } 
  else if (strcmp(operation.name(), "INT_COMPARE") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return callOperationForDatatype(data, args, std::equal_to<>());
      });
  } 
  else if (strcmp(operation.name(), "LOGICAL_AND") == 0) {
    function =
      ([this](std::vector<uint8_t> data, std::vector<ModifierFunction> args) {
        return callOperationForDatatype(data, args, std::logical_and<>());
      });
  } 
  else if (strcmp(operation.name(), "CONSTANT") == 0) {
    int const const_value = operation.attribute("value").as_int();
    function = ([this, const_value](std::vector<uint8_t>,
                                    std::vector<ModifierFunction>) {
      std::vector<uint8_t> value = std::vector<uint8_t>(sizeof(const_value));
      copyTypeToData(const_value, value);
      return value;
    });
    
  } 
  else {
    std::cout << "Unsupported function " << operation.name() << std::endl;
  }
}

std::vector<uint8_t> ModifierFunction::modifierSelectByte(std::vector<uint8_t> data, std::vector<ModifierFunction> args)
{
  auto const byte_start = convertDataToType<uint16_t>(args[0].call(data));
  auto const byte_end = convertDataToType<uint16_t>(args[1].call(data));
  if (byte_start > byte_end || (byte_end - byte_start) > max_value_size) {
    throw std::runtime_error("byte_start must be smaller than byte_end");
    }
    return { data.begin() + byte_start, data.begin() + byte_end };
}

template<typename T, typename Op>
std::vector<uint8_t> ModifierFunction::applyOperation(std::vector<uint8_t> data, T argument1, Op operation) {
    T value = convertDataToType<T>(data);
    value = operation(value, static_cast<T>(argument1));  // Apply the bitwise operation using the passed operation
    copyTypeToData(value, data);
    return data;
}

template<typename Op>
std::vector<uint8_t> ModifierFunction::callOperationForDatatype(std::vector<uint8_t> data, std::vector<ModifierFunction> args, Op operation)
{

  std::vector<uint8_t> const input_value = args[0].call(data);
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

template<typename T>
std::vector<uint8_t> ModifierFunction::applyBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift) {
    T value = convertDataToType<T>(data);
    if (isLeftShift) {
        value <<= nr_bits;
    } else {
        value >>= nr_bits;
    }
    copyTypeToData(value, data);
    return data;
}

std::vector<uint8_t> ModifierFunction::modifierBitShift(std::vector<uint8_t> data, std::vector<ModifierFunction> args, bool isLeftShift)
{

  std::vector<uint8_t> const value_to_shift = args[0].call(data);
  auto const nr_bits = convertDataToType<uint16_t>(args[1].call(data));
  switch (value_to_shift.size()) {
    case sizeof(uint8_t):
      return applyBitShift<uint8_t>(value_to_shift, nr_bits, isLeftShift);
    case sizeof(uint16_t):
      return applyBitShift<uint16_t>(value_to_shift, nr_bits, isLeftShift);
    case sizeof(uint32_t):
      return applyBitShift<uint32_t>(value_to_shift, nr_bits, isLeftShift);
    case sizeof(uint64_t):
      return applyBitShift<uint64_t>(value_to_shift, nr_bits, isLeftShift);
    default:
      throw std::runtime_error("data must be exactly the size of a default "
                               "integer type (1,2,4 or 8 bytes)");
    }
}

template<typename T>
T ModifierFunction::convertDataToType(const std::vector<uint8_t>& data) {
    T value = 0;
    std::memcpy(&value, data.data(), data.size());
    return value;
}

template<typename T>
void ModifierFunction::copyTypeToData(T value, std::vector<uint8_t>& data) {
    std::memcpy(data.data(), &value, sizeof(T));
}

std::vector<uint8_t> ModifierFunction::call(std::vector<uint8_t> can_data) {
    return this->function(can_data, this->arguments);
}