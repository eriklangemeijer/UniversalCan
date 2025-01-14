#pragma once

#include <vector>
#include <string>
#include <memory>
#include <pugixml.hpp>

#include <ModifierFunction.h>
class CanValueTemplate
{
    private:
        std::string value_name;
        std::string data_type ;
        std::shared_ptr<ModifierFunction> root_function;

    public:        
        CanValueTemplate(pugi::xml_node template_description);
        std::vector<uint8_t> parseData(std::vector<uint8_t> can_data);

};