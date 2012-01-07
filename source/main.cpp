#include <iostream>

#include <Windows.h>

#include "console.hpp"
#include "exception.hpp"
#include "hook.hpp"
#include "hooks.hpp"
#include "patches.hpp"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		initialiseConsole();
		installExceptionHandler();
		installHooks();
		activateDebugRegisters();
		//this is currently not safe because threads are not suspended while the hot patches are being applied
		installHotPatches();
	}
	return TRUE;
}