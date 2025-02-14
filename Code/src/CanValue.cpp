
#include "CanValueTemplate.h"
#include <CanValue.h>
#include <cstdint>
#include <string>
#include <vector>

CanValue::CanValue(CanValueTemplate &template_description, std::vector<uint8_t> can_data) : value_name(template_description.getValueName()),
                                                                                            data_type(template_description.getDataType()) {
    value = template_description.parseData(can_data);
}

std::string CanValue::getValueName() const {
    return value_name;
}

std::string CanValue::getDataType() const {
    return data_type;
}

bool CanValue::getBoolValue() const {
    return value[0] != 0;
}
