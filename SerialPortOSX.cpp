#include "SerialPortOSX.h"


void locCallBack(int status);
SerialPortOSX::SerialPortOSX(std::string location)
{

}


SerialPortOSX::~SerialPortOSX()
{

}

void SerialPortOSX::registerCallback(std::function<void(std::vector<char>)> callback)
{

}

bool SerialPortOSX::sendMessage(std::vector<char> data)
{
    return true;
}

void locCallBack(int status)
{

}