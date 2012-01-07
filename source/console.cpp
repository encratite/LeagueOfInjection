#include <iostream>

#include <cstdio>

#include <ail/string.hpp>

#include <Windows.h>

namespace
{
	HANDLE consoleHandle;
}

void initialiseConsole()
{
	AllocConsole();
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	std::cout.sync_with_stdio();
	std::cin.sync_with_stdio();
}

void writeText(std::string const & text)
{
	DWORD bytesWritten;
	WriteConsole(consoleHandle, reinterpret_cast<void const *>(text.c_str()), static_cast<DWORD>(text.size()), &bytesWritten, 0);
}

void writeLine(std::string const & text)
{
	writeText(text + "\n");
}