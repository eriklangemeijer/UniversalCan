
#include <stdio.h>
#include "SerialPortOSX.h"
#include "xmlReader.h"
int main()
{
    // SerialPortOSX foo("/dev/tty.usbserial-1420");
    // printf("hoi");
    // while(1)
    //     foo.sendMessage(std::vector<char>({'s','s','s'}));
    xmlReader reader("./Databases/BMW_GS.csv");

}