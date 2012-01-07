#include <iostream>

#include <Windows.h>

#include "console.hpp"
#include "exception.hpp"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		initialiseConsole();
		installExceptionHandler();
	}
	return TRUE;
}