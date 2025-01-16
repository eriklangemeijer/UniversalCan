#pragma once

#include <CanValue.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <list>
#include <pugixml.hpp>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

class CanMessageTemplate {
  private:
    std::string name;
    std::string description;
    std::list<CanValueTemplate> value_list;
    std::shared_ptr<ModifierFunction> filter_function;

  public:
    CanMessageTemplate(pugi::xml_node &msg);
    std::vector<CanValue> parseData(std::vector<uint8_t> can_data);
    bool isMatch(std::vector<uint8_t> can_data);
    std::string getName();
};