#include "ITransciever.h"
#include "ISerial.h"
class ELM327 : public ITransciever
{

	private:
		std::unique_ptr<ISerial> serial;
		bool running;
	
	public:
		ELM327(std::unique_ptr<ISerial> serial) ;
		~ELM327();
		bool sendMessage(std::vector<CanMessage> messages) override;
		void registerCallback(std::function<void(CanMessage)> callback) override;
		void threadFunction(std::function<void(CanMessage)> callback, HANDLE comPort);
		void serialReceiveCallback(std::vector<uint8_t>);
};