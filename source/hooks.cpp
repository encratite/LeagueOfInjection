#include <iostream>

#include <cstdint>

#include <Windows.h>

#include <ail/file.hpp>
#include <ail/string.hpp>

#include "hook.hpp"
#include "patches.hpp"

namespace
{
	std::string const logPath = "LoLI.log";
	ail::file logFile;
}

template <typename ValueType>
	ValueType loadFromStack(CONTEXT & threadContext, int offset)
{
	return *reinterpret_cast<ValueType *>(threadContext.Esp + offset);
}

void writeLog(std::string const & input)
{
	if(logFile.open())
		logFile.write(input);
	else
	{
		bool success = logFile.open_create("..\\..\\..\\..\\..\\" + logPath);
		if(success)
			std::cout << "Started logging to " << logPath << std::endl;
		else
			std::cout << "Unable to open " << logPath << std::endl;
	}
}

void printBuffer(char * buffer, std::size_t bufferSize)
{
	std::string data(buffer, bufferSize);
	for(std::size_t i = 0; i < data.size(); i++)
	{
		char input = data[i];
		if(input >= ' ' && input <= '~')
			std::cout << input;
		else
			std::cout << '?';
	}
	std::cout << std::endl;
}

void encryptTLSHandler(CONTEXT & threadContext)
{
	char * buffer = loadFromStack<char *>(threadContext, 4);
	std::size_t bufferSize = loadFromStack<std::size_t>(threadContext, 8);
	//emulate push 40 and skip the execution of the instruction
	threadContext.Esp -= 4;
	*reinterpret_cast<uint32_t *>(threadContext.Esp) = 0x40;
	threadContext.Eip += 2;
	std::cout << "Encrypted " << bufferSize << " bytes" << std::endl;
	std::string data(buffer, bufferSize);
	std::string logData = "Encrypted " + ail::number_to_string(bufferSize) + " bytes:\n" + data + "\n";
	writeLog(logData);
}

void decryptTLSHandler(CONTEXT & threadContext)
{
	char * buffer = loadFromStack<char *>(threadContext, 4);
	std::size_t bufferSize = loadFromStack<std::size_t>(threadContext, 8);
	//emulate retn 8
	DWORD returnAddress = loadFromStack<DWORD>(threadContext, 0);
	threadContext.Eip = returnAddress;
	threadContext.Esp += 3 * 4;
	if(buffer == NULL)
		return;
	std::cout << "Decrypted " << bufferSize << " bytes" << std::endl;
	std::string data(buffer, bufferSize);
	std::string logData = "Decrypted " + ail::number_to_string(bufferSize) + " bytes:\n" + data + "\n";
	writeLog(logData);
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
	DWORD const
		encryptTLS = 0x101EF0A3,
		decryptTLS = 0x101EEF87;
	installHook(Hook("encryptTLS", encryptTLS, staticBase, dynamicBase, &encryptTLSHandler));
	installHook(Hook("decryptTLS", decryptTLS, staticBase, dynamicBase, &decryptTLSHandler));
}