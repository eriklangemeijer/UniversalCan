#include "ITransciever.h"
#include <Transcievers/ISerial.h>
#include <memory>
#include <string>
#include <list>
#include <mutex>

class ELM327 : public ITransciever
{

	private:
		std::unique_ptr<ISerial> serial;
		bool running;
		std::function<void(CanMessage)> callbackFunction;
    	void threadFunction();
		std::list<std::string> messageList;
		std::mutex messageListLock;

		void storeMessage(std::string message);
		bool ready;

		bool sendATMessage(std::string command, bool waitForResponse=true);
	
	public:
		explicit ELM327(std::unique_ptr<ISerial> serial);
		~ELM327();

		void start() override;
		bool sendMessage(std::vector<CanMessage> messages) override;
		void registerCallback(std::function<void(CanMessage)> callback) override;
		// void threadFunction(std::function<void(CanMessage)> callback, std::string comPort);
		void serialReceiveCallback(const std::string& message);
		bool messageAvailable();
		std::string readMessage();
};