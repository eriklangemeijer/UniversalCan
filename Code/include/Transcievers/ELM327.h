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
    std::unique_ptr<ProtocolDefinitionParser> protocol_parser;
    bool running;
    std::function<void(CanMessage)> callbackFunction;
    void threadFunction();
    std::list<CanMessage> messageList;
    std::mutex messageListLock;

    void storeMessage(CanMessage &message);
    bool ready;

    bool sendATMessage(std::string command, bool waitForResponse = true);

  public:
    explicit ELM327(std::unique_ptr<ISerial> serial, std::unique_ptr<ProtocolDefinitionParser> protocol_parser);
    ~ELM327();

    void start() override;
    bool sendMessage(std::vector<CanMessage> messages) override;
    void registerCallback(std::function<void(CanMessage)> callback) override;
    void serialReceiveCallback(std::vector<uint8_t> message);
    bool messageAvailable();
    std::shared_ptr<CanMessage> readMessage();
};