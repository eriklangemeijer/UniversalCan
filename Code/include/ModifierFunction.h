#pragma once

#include <functional>
#include <list>
#include <pugixml.hpp>
#include <stdint.h>
#include <string>
#include <vector>
class ModifierFunction {
  private:
    std::function<std::vector<uint8_t>(std::vector<uint8_t>, std::vector<ModifierFunction>)> function;
    std::vector<ModifierFunction> arguments;
    std::string name;

    template <typename T>
    std::vector<uint8_t> applyBitShift(std::vector<uint8_t> data, uint64_t nr_bits, bool isLeftShift);
    template <typename T, typename Op>
    std::vector<uint8_t> applyOperation(std::vector<uint8_t> data,
                                        T argument1,
                                        Op operation);
    template <typename T>
    void copyTypeToData(T value, std::vector<uint8_t> &data);
    template <typename T>
    T convertDataToType(const std::vector<uint8_t> &data);

  public:
    ModifierFunction(pugi::xml_attribute attribute);
    ModifierFunction(pugi::xml_node operation);
    std::vector<uint8_t> modifierSelectByte(std::vector<uint8_t> data, std::vector<ModifierFunction> args);
    template <typename Op>
    std::vector<uint8_t> callOperationForDatatype(std::vector<uint8_t> data, std::vector<ModifierFunction> args, Op operation);
    std::vector<uint8_t> modifierBitShift(std::vector<uint8_t> data, std::vector<ModifierFunction> args, bool isLeftShift);
    std::vector<uint8_t> call(std::vector<uint8_t> can_data);
};