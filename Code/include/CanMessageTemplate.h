#pragma once

#include <stdint.h>
#include <vector>
#include <string>
class CanMessageTemplate
{
    private:

    public:
        CanMessageTemplate();
        std::vector<std::string, void*> getValueList(std::vector<uint8_t> can_data);

};