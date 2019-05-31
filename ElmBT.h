
#include <String>

#include <vector>
#include <memory>
#include <functional>
#include "IAdapter.h"
#include "ISerialPort.h"
class ElmBT : public IAdapter
{
	private:
		std::function<void(CanMessage)> callbackFunction;
		std::unique_ptr<ISerialPort> comPort;
		bool running;
		std::string version;
	public:
		ElmBT(std::string port);
		~ElmBT();
		bool sendMessage(std::vector<CanMessage> messages) override;
		void registerCallback(std::function<void(CanMessage)> callback) override;
		void serialCallback(std::string message);
};