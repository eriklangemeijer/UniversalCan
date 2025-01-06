#include "ITransciever.h"
#include "ISerial.h"
#include <memory>
#include <String>
class ELM327 : public ITransciever
{

	private:
		std::unique_ptr<ISerial> serial;
		bool running;
		std::function<void(CanMessage)> callbackFunction;
    	void threadFunction();
	
	public:
		ELM327(std::unique_ptr<ISerial> serial) ;
		~ELM327();

		void start() override;
		bool sendMessage(std::vector<CanMessage> messages) override;
		void registerCallback(std::function<void(CanMessage)> callback) override;
		// void threadFunction(std::function<void(CanMessage)> callback, std::string comPort);
		void serialReceiveCallback(std::string message);
};