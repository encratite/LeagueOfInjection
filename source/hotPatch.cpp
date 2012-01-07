#include <iostream>

#include <Windows.h>

#include <ail/memory.hpp>

bool hotPatchFunction(std::string const & module, std::string const & function, void * functionPointer, void * & oldAddress)
{
	HMODULE moduleHandle = GetModuleHandle(module.c_str());
	if(moduleHandle == 0)
	{
		std::cout << "Unable to retrieve " << module << " module handle" << std::endl;
		return false;
	}

	char * address = reinterpret_cast<char *>(GetProcAddress(moduleHandle, function.c_str()));
	if(address == NULL)
	{
		std::cout << "Unable to get procedure address of " << function << " in module " << module << std::endl;
		return false;
	}

	oldAddress = address + 2;

	std::string expectedBytes = "\x8b\xff";
	std::string procedureBytes;
	procedureBytes.assign(address, expectedBytes.length());

	if(procedureBytes != expectedBytes)
	{
		std::cout << "Unable to patch " << function << " in module " << module << " - are you running a pre Windows XP SP1 operating system by Microsoft?" << std::endl;
		return false;
	}

	uint32_t functionAddress = reinterpret_cast<uint32_t>(functionPointer) - reinterpret_cast<uint32_t>(address);
	std::string replacement_string = ail::little_endian_string(functionAddress, 4);
	std::string replacement = "\xe9" + replacement_string + "\xeb\xf9";

	char * offset = address - 5;

	DWORD oldProtection;
	if(VirtualProtect(offset, replacement.length(), PAGE_EXECUTE_READWRITE, &oldProtection) == 0)
	{
		std::cout << "Unable to patch " << function << " in module " << module << " because I was unable to make it writable" << std::endl;
		return false;
	}

	std::memcpy(offset, replacement.c_str(), replacement.length());

	DWORD unused;
	if(VirtualProtect(offset, replacement.length(), oldProtection, &unused) == 0)
	{
		std::cout << "Unable to protect our patch of " << function << " in module " << module << std::endl;
		return false;
	}

	return true;
}