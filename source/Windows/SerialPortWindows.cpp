#include "SerialPortWindows.h"
#include <thread>
SerialPortWindows::SerialPortWindows(std::string port):running(false)
{
	comPort = CreateFileA(port.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	DCB comParams;
	if (comPort == INVALID_HANDLE_VALUE  || !GetCommState(comPort, &comParams))
		throw ERROR_CONNECTION_INVALID;

	comParams.BaudRate = CBR_9600;
	comParams.ByteSize = 8;
	comParams.StopBits = ONESTOPBIT;
	comParams.Parity = NOPARITY;
	comParams.fDtrControl = DTR_CONTROL_DISABLE;
	comParams.EvtChar = '>';
	if (!SetCommState(comPort, &comParams))
		throw ERROR_CONNECTION_INVALID;

	//PurgeComm(comPort, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

SerialPortWindows::~SerialPortWindows()
{
    running = false;
	CloseHandle(comPort);
}

void SerialPortWindows::threadFunction(std::function<void(std::string)> callback, HANDLE comPort)
{
	printf("hello");
	SetCommMask(comPort, EV_RXCHAR );
	OVERLAPPED ov;
	memset(&ov, 0, sizeof(ov));
	ov.hEvent = CreateEvent(0, true, 0, 0);
	unsigned long reason;
	while (running)
	{
		std::this_thread::yield();
		//
		uint8_t buffer[300];
		unsigned long bytesRead;
		memset(buffer, 0, sizeof(buffer));
		sendString("ATZ\n\r");
		WaitCommEvent(comPort, &reason, &ov);
		if (WaitForSingleObject(ov.hEvent, 10) == WAIT_OBJECT_0)
		{
			ReadFile(comPort, buffer, sizeof(buffer), &bytesRead, &ov);
			printf("%s", buffer);
			uint16_t id = 0;//to be calced
			if ((id > 0x7DF) && (id < 0x7EF))
				printf("got obd message");
		}
	}
}

bool SerialPortWindows::sendMessage(std::vector<char> data)
{
	return false;
}

bool SerialPortWindows::sendString(std::string data)
{
	DWORD bytesSend;

	if (!WriteFile(comPort, data.c_str(), data.size(), &bytesSend, 0)) {
		return false;
	}
	else return true;
}

void SerialPortWindows::registerCallback(std::function<void(std::string)> callback)
{
	auto callbackFunction = callback;
	running = true;
	uint8_t buffer[300];
	unsigned long bytesRead;
	memset(buffer, 0, sizeof(buffer));
	OVERLAPPED ov = {0};
	ov.hEvent = CreateEvent(0, true, 0, 0);
	while (running) {
		sendString("ATZ\n\r");
		ReadFile(comPort, buffer, sizeof(buffer), &bytesRead, &ov);
	}
	//std::thread thread_obj(&SerialPortWindows::threadFunction,this,callback, comPort);
	//thread_obj.join();
}