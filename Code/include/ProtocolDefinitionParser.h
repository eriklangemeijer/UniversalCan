#pragma once

#include <CanMessageTemplate.h>
class ProtocolDefinitionParser {
    private:

        std::string canonicalize_path(const std::string& path);
        std::string get_parent_path(const std::string& path);
        std::string combine_paths(const std::string& base, const std::string& relative);
        bool file_exists(const std::string& path);

    public:
        ProtocolDefinitionParser(std::string filename);
        std::list<CanMessageTemplate> message_list;
};