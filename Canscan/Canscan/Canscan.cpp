// Canscan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <windef.h>
#include "..\..\ElmBT.h"

void messageCallback(CanMessage message)
{

}
int main()
{
	ElmBT elm("COM4");
	elm.registerCallback(&messageCallback);
	printf("foo");
	while (1);
    return 0;
}

