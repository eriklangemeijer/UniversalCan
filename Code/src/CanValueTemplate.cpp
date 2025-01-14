#include <CanValueTemplate.h>


CanValueTemplate::CanValueTemplate(pugi::xml_node template_description)
{
    value_name = template_description.attribute("name").as_string();
    data_type = template_description.attribute("DataType").as_string();
    root_function = std::make_shared<ModifierFunction>(template_description.first_child());
}

std::vector<uint8_t> CanValueTemplate::parseData(std::vector<uint8_t> can_data)
{   
    return this->root_function->call(can_data);
}


