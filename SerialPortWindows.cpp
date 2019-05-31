#include "SerialPortWindows.h"

SerialPortWindows::SerialPortWindows():running(false)
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
	comParams.fDtrControl = DTR_CONTROL_ENABLE;
	comParams.EvtChar = '\n';
	if (!SetCommState(comPort, &comParams))
		throw ERROR_CONNECTION_INVALID;

	PurgeComm(comPort, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

SerialPortWindows::~SerialPortWindows()
{
    running = false;
	CloseHandle(comPort);
}

void SerialPortWindows::threadFunction(std::function<void(std::vector<char>) callback, HANDLE comPort)
{
	printf("hello");
	SetCommMask(comPort, EV_RXCHAR | EV_RXFLAG);
	OVERLAPPED ov;
	memset(&ov, 0, sizeof(ov));
	ov.hEvent = CreateEvent(0, true, 0, 0);
	unsigned long reason;
	while (running)
	{
		WaitCommEvent(comPort, &reason, &ov);
		if (WaitForSingleObject(ov.hEvent, INFINITE) == WAIT_OBJECT_0)
		{
			uint8_t buffer[sizeof(CanMessage)];
			unsigned long bytesRead;
			memset(buffer, 0, sizeof(buffer));
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

}

void SerialPortWindows::registerCallback(std::function<void(std::vector<char>)> callback)
{
	callbackFunction = callback;
	running = true;
	std::thread thread_obj(&SerialPortWindows::threadFunction,this,callback, comPort);
	thread_obj.join();
}