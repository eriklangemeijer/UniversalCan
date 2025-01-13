#include <ProtocolDefinitionParser.h>
#include <experimental/filesystem> // For file path manipulation
#include <stdexcept>  // For runtime_error

namespace fs = std::experimental::filesystem;

ProtocolDefinitionParser::ProtocolDefinitionParser(std::string filename) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    if (!result)
        throw std::runtime_error("Cannot open protocol file");


    for (auto message = doc.child("ProtocolDefintion").child("Message"); message; message = message.next_sibling("Message")) {
        message_list.emplace_back(CanMessageTemplate(message));
    }
    fs::path base_path = fs::absolute(fs::path(filename)).parent_path();

    for (auto included_document = doc.child("ProtocolDefintion").child("include"); included_document; included_document = included_document.next_sibling("include")) {
        std::string relative_path = included_document.attribute("path").as_string();

        fs::path absolute_path = base_path / relative_path;
        absolute_path = fs::absolute(absolute_path);

        if (!fs::exists(absolute_path)) {
            throw std::runtime_error("Included file does not exist: " + absolute_path.string());
        }
        ProtocolDefinitionParser parent_protocol = ProtocolDefinitionParser(absolute_path.string());
        // message_list.splice(message_list.end(), parent_protocol.message_list);
    }
}
