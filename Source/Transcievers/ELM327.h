#include "ITransciever.h"
class ELM327 : public ITransciever
{

	private:
	
	public:
		

		ELM327(std::string port);
		~ELM327();
		bool sendMessage(std::vector<CanMessage> messages) override;
		void registerCallback(std::function<void(CanMessage)> callback) override;
		void threadFunction(std::function<void(CanMessage)> callback, HANDLE comPort);
};