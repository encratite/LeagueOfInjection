#include <iostream>

#include <Windows.h>

#include "console.hpp"
#include "exception.hpp"
#include "hook.hpp"

void encryptTLSHandler(CONTEXT & threadContext)
{
	//emulate push 40 and skip the execution of the instruction
	threadContext.Esp -= 4;
	*reinterpret_cast<DWORD *>(threadContext.Esp) = 0x40;
	threadContext.Eip += 2;
}

void installHooks()
{
	HMODULE module = GetModuleHandle("Adobe AIR.dll");
	if(module == NULL)
	{
		std::cout << "Unable to locate Adobe AIR module" << std::endl;
		return;
	}
	DWORD dynamicBase = reinterpret_cast<DWORD>(module);
	DWORD const staticBase = 0x10000000;
	DWORD const encryptTLS = 0x101EF0A3;
	installHook(Hook("encryptTLS", encryptTLS, staticBase, dynamicBase, &encryptTLSHandler));
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