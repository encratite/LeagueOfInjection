#include <iostream>

#include <Windows.h>

#include "console.hpp"
#include "exception.hpp"
#include "hook.hpp"

void installHooks()
{
	HMODULE module = GetModuleHandle("Adobe AIR.dll");
	if(module == NULL)
	{
		std::cout << "Unable to locate Adobe AIR module" << std::endl;
		return;
	}
	
	std::cout << "Handle: " << (void *)module << std::endl;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		initialiseConsole();
		installExceptionHandler();
		installHooks();
		activateDebugRegisters();
	}
	return TRUE;
}