#pragma once

#include <CanMessageTemplate.h>
class ProtocolDefinitionParser {
    private:

    public:
        ProtocolDefinitionParser(std::string filename);
        std::list<CanMessageTemplate> message_list;
};