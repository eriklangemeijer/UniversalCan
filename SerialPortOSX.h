#include "ISerialPort.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <errno.h>
#include <termios.h>

#include <string.h>
#include <sstream>
#include <iostream>
#define BUFFERSIZE 1024
class SerialPortOSX : public ISerialPort
{
    private:
        struct termios termAttr;
        struct sigaction saio;
        int serialFile;
		bool running;
        void threadFunction(std::function<void(std::vector<char>)> callback);
        char buffer[BUFFERSIZE];

    public:
        SerialPortOSX();
        ~SerialPortOSX();
        bool sendMessage(std::vector<char> data);
        bool sendString(std::string data);
        void registerCallback(std::function<void(std::string)> callback);
};