
#include <windows.h>

#include "ISerialPort.h"
#include "CanMessage.h"

class SerialPortWindows : public ISerialPort
{
    private:
		HANDLE comPort;
		bool running;
		void threadFunction(std::function<void(std::string)> callback, HANDLE comPort);

    public:
        SerialPortWindows(std::string port);
		~SerialPortWindows();
        bool sendMessage(std::vector<char> data);
		bool sendString(std::string data);
        void registerCallback(std::function<void(std::string)> callback);
};