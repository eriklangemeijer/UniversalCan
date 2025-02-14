#pragma once

#include <CanMessageTemplate.h>
class ProtocolDefinitionParser {
  private:
    static std::string canonicalizePath(const std::string &path);
    static std::string getParentPath(const std::string &path);
    static std::string combinePaths(const std::string &base,
                                    const std::string &relative);
    static bool fileExists(const std::string &path);
    std::list<CanMessageTemplate> message_list;

  public:
    ProtocolDefinitionParser(std::string filename);
    std::list<CanMessageTemplate> getMessageList();
    std::shared_ptr<CanMessageTemplate> findMatch(std::vector<uint8_t> can_data);
    std::shared_ptr<CanMessageTemplate> findMessageByName(std::string msg_name);
};