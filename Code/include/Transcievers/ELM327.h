#include <ProtocolDefinitionParser.h>
#include <Transcievers/ISerial.h>
#include <Transcievers/ITransciever.h>
#include <list>
#include <memory>
#include <mutex>
#include <string>

class ELM327 : public ITransciever {

  private:
    std::unique_ptr<ISerial> serial;
    std::unique_ptr<ProtocolDefinitionParser> protocol_definition;
    bool running;
    void threadFunction();
    std::list<CanMessage> message_list;
    std::mutex message_list_lock;

    void storeMessage(CanMessage &message);
    bool ready;

    bool sendATMessage(std::string command, bool wait_for_response = true);
    static std::vector<uint8_t> hexStringToBytes(std::string message_str);

  public:
    explicit ELM327(std::unique_ptr<ISerial> serial, std::unique_ptr<ProtocolDefinitionParser> protocol_definition);
    ~ELM327();

    void start() override;
    bool sendMessages(std::vector<CanMessage> messages) override;
    bool requestMessage(std::string msg_name);
    void serialReceiveCallback(std::vector<uint8_t> message);
    bool messageAvailable();
    std::shared_ptr<CanMessage> readMessage();
};