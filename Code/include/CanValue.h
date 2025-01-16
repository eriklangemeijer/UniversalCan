#pragma once

#include <memory>
#include <string>
#include <vector>

#include <CanValueTemplate.h>

class CanValue {
  private:
    std::string value_name;
    std::string data_type;
    std::vector<uint8_t> value;

  public:
    CanValue(CanValueTemplate &template_description, std::vector<uint8_t> can_data);

    // Getters
    std::string getValueName() const;
    std::string getDataType() const;
    bool getBoolValue() const;

    // Template function cannot be splitted to cpp file because of linker errors.
    template <typename T>
    T getValue() {
        if (sizeof(T) != value.size()) {
            throw std::runtime_error("Size of value does not match size of requested type");
        } else {
            return *reinterpret_cast<const T *>(value.data());
        }
    }
};