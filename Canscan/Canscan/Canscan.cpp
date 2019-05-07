// Canscan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <windef.h>
#include "..\..\ElmWindows.h"

void messageCallback(CanMessage message)
{

}
int main()
{
	ElmWindows elm("COM2");
	elm.registerCallback(&messageCallback);
	printf("foo");
	while (1);
    return 0;
}

