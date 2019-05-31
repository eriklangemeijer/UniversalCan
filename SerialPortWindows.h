#include "ISerialPort.h"
#include <windows.h>

class SerialPortWindows : public ISerialPort
{
    private:
		HANDLE comPort;
		bool running;
        void threadFunction(std::function<void(std::vector<char>) callback HANDLE comPort);

    public:
        SerialPortWindows();
        bool sendMessage(std::vector<char> data);
        void registerCallback(std::function<void(std::vector<char>)> callback);
};