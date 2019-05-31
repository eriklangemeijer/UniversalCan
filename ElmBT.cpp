#include "ElmBT.h"
#include "SerialPortOSX.h"
#include <thread>

void ElmBT::serialCallback(std::string message)
{
	if(!running && message.find(">"))
	{
		if(sscanf(message.c_str,"ELM327 %s",version))
		{
			std::cout << "Found an adapter with version " << version << std::endl;
			running = true;
		}
		else
			std::cout << "Could not verify version of connected adapter.";

		return;
	}
}

ElmBT::ElmBT(std::string port) :
	running(false)
{

	comPort = new SerialPortOSX();
	comPort->registerCallback(std::bind(&(ElmBT::serialCallback),this,std::placeholders::_1));

	comPort->sendString("ATZ");

}

ElmBT::~ElmBT()
{
	
}

bool ElmBT::sendMessage(std::vector<CanMessage> messages)
{
	if(!running)
		return false;
}

void ElmBT::registerCallback(std::function<void(CanMessage)> callback)
{
}
