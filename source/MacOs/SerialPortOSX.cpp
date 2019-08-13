#include "SerialPortOSX.h"


SerialPortOSX::SerialPortOSX()
{
serialFile = open( "sda", O_RDWR| O_NOCTTY );
struct termios tty;
struct termios tty_old;
memset (&tty, 0, sizeof tty);

/* Error Handling */
if ( tcgetattr ( serialFile, &tty ) != 0 ) {
   std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
}

/* Save old tty parameters */
tty_old = tty;

/* Set Baud Rate */
cfsetospeed (&tty, (speed_t)B9600);
cfsetispeed (&tty, (speed_t)B9600);

/* Setting other Port Stuff */
tty.c_cflag     &=  ~PARENB;            // Make 8n1
tty.c_cflag     &=  ~CSTOPB;
tty.c_cflag     &=  ~CSIZE;
tty.c_cflag     |=  CS8;

tty.c_cflag     &=  ~CRTSCTS;           // no flow control
tty.c_cc[VMIN]   =  1;                  // read doesn't block
tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

/* Make raw */
cfmakeraw(&tty);

/* Flush Port, then applies attributes */
tcflush( serialFile, TCIFLUSH );
if ( tcsetattr ( serialFile, TCSANOW, &tty ) != 0) {
   std::cout << "Error " << errno << " from tcsetattr" << std::endl;
}
}


SerialPortOSX::~SerialPortOSX()
{

}

void SerialPortOSX::registerCallback(std::function<void(std::string)> callback)
{
    int n = 0,
        spot = 0;
    char buf = '\0';

    /* Whole response*/
    char response[1024];
    memset(response, '\0', sizeof response);

    do {
        n = read( serialFile, &buf, 1 );
        sprintf( &response[spot], "%c", buf );
        spot += n;
    } while( buf != '\r' && n > 0);

    if (n < 0) {
        std::cout << "Error reading: " << strerror(errno) << std::endl;
    }
    else if (n == 0) {
        std::cout << "Read nothing!" << std::endl;
    }
    else {
        std::cout << "Response: " << response << std::endl;
    }
}

bool SerialPortOSX::sendString(std::string data)
{
    int n_written = 0;

    n_written = write( serialFile, data.c_str(), data.size() );
    return false;
}

void locCallBack(int status)
{

}
bool sendMessage(std::vector<char> data)
{
    return false;
}