#include "IAdapter.h"
#include <windows.h>
class ElmWindows : public IAdapter
{

	private:
		std::function<void(CanMessage)> callbackFunction;
		HANDLE comPort;
		bool running;
	
	public:
		

		ElmWindows(std::string port);
		~ElmWindows();
		bool sendMessage(std::vector<CanMessage> messages) override;
		void registerCallback(std::function<void(CanMessage)> callback) override;
		void threadFunction(std::function<void(CanMessage)> callback, HANDLE comPort);
};