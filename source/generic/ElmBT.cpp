#include "ElmBT.h"
#include "ISerialPort.h"
#include <thread>
#include <iostream>
#include <memory>
void ElmBT::serialCallback(std::string message)
{
	if(!running && message.find(">"))
	{
		/*
		char buff[20];
		if(sscanf(message.c_str,"ELM327 %s", buff))
		{
			std::cout << "Found an adapter with version " << buff << std::endl;
			running = true;
		}
		else
			std::cout << "Could not verify version of connected adapter.";
		
		return;
		*/
	}
}

ElmBT::ElmBT(std::unique_ptr<ISerialPort> comPort) :
	running(false)
{

	this->comPort =  std::move(comPort);
    this->comPort->sendString("ATZ\n\r");
    this->comPort->registerCallback(std::bind(&ElmBT::serialCallback, this,
		std::placeholders::_1));


}

ElmBT::~ElmBT()
{
	
}

bool ElmBT::sendMessage(std::vector<CanMessage> messages)
{
	if(!running)
		return false;
	return true;
}

void ElmBT::registerCallback(std::function<void(CanMessage)> callback)
{
}
