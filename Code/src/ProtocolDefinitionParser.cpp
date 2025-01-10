#include <ProtocolDefinitionParser.h>




ProtocolDefinitionParser::ProtocolDefinitionParser(std::string filename) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    if (!result)
        throw std::runtime_error("Cannot open protocol file");

    for (auto message = doc.child("ProtocolDefintion").child("Message"); message; message = message.next_sibling("Message")) {
        message_list.emplace_back(CanMessageTemplate(message));
    }

}