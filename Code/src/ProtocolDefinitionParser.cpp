#include "CanMessageTemplate.h"
#include <ProtocolDefinitionParser.h>
#include <cstdlib>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

ProtocolDefinitionParser::ProtocolDefinitionParser(std::string filename) {
    pugi::xml_document doc;
    pugi::xml_parse_result const result = doc.load_file(filename.c_str());
    if (!result)
        throw std::runtime_error("Cannot open protocol file");

    for (auto message = doc.child("ProtocolDefintion").child("Message"); message; message = message.next_sibling("Message")) {
      message_list.emplace_back(message);
    }

    std::string const base_path = getParentPath(canonicalizePath(filename));

    for (auto included_document = doc.child("ProtocolDefintion").child("include"); included_document; included_document = included_document.next_sibling("include")) {
      std::string const relative_path =
        included_document.attribute("path").as_string();

      std::string const absolute_path = combinePaths(base_path, relative_path);

      if (!fileExists(absolute_path)) {
        throw std::runtime_error("Included file does not exist: " +
                                 absolute_path);
        }
        auto parent_list = ProtocolDefinitionParser(absolute_path).message_list;
        message_list.insert(message_list.end(), parent_list.begin(), parent_list.end());
    }
}

std::string ProtocolDefinitionParser::canonicalizePath(const std::string& path) {
    char resolved_path[4096];
#ifdef _WIN32
    if (_fullpath(resolved_path, path.c_str(), sizeof(resolved_path)) == nullptr) {
        throw std::runtime_error("Failed to resolve path: " + path);
    }
#else
    if (realpath(path.c_str(), resolved_path) == nullptr) {
        throw std::runtime_error("Failed to resolve path: " + path);
    }
#endif
    return { resolved_path };
}

std::string ProtocolDefinitionParser::getParentPath(const std::string& path) {
  size_t const last_slash = path.find_last_of("/\\");
  if (last_slash == std::string::npos) {
    return "";
    }
    return path.substr(0, last_slash);
}

std::string ProtocolDefinitionParser::combinePaths(const std::string& base, const std::string& relative) {
    if (relative.empty()) {
        return base;
    }
    if (relative[0] != '.') {
        throw std::runtime_error("Include path is not a relative path. Please start the path name with ./");
    }
    return base + "/" + relative;
}

bool ProtocolDefinitionParser::fileExists(const std::string& path) {
  std::ifstream const file(path);
  return file.good();
}

std::list<CanMessageTemplate> ProtocolDefinitionParser::getMessageList() {
    return message_list;
}

std::shared_ptr<CanMessageTemplate> ProtocolDefinitionParser::findMatch(std::vector<uint8_t> can_data) {
    for (auto message : this->message_list) {
        if (message.isMatch(can_data)) {
            return std::make_shared<CanMessageTemplate>(message); // Return a reference to the matching message
        }
    }
    return nullptr;
}