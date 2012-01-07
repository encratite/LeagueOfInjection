#include <iostream>

#include <Windows.h>
#include <TlHelp32.h>

#include <ail/string.hpp>

#include "hook.hpp"

Hooks hooks;

namespace
{
	std::size_t const maximumHookCount = 4;
}

Hook::Hook(std::string const & description, DWORD address, HookFunction handler):
	description(description),
	address(address),
	handler(handler)
{
}

void installHook(Hook const & hook)
{
	if(hooks.size() < maximumHookCount)
		hooks.push_back(hook);
	else
		std::cout << "Maximum hook count exceeded" << std::endl;
}

void setDebugRegisters(CONTEXT & threadContext)
{
	unsigned const
		debugRegisterTypeExecute = 0,
		debugRegisterSizeOne = 0;

	DWORD * debugRegisters[] =
	{
		&threadContext.Dr0,
		&threadContext.Dr1,
		&threadContext.Dr2,
		&threadContext.Dr3
	};

	threadContext.Dr6 = 0;
	threadContext.Dr7 = 0;

	threadContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	for(std::size_t i = 0; i < maximumHookCount && i < hooks.size(); i++)
	{
		*(debugRegisters[i]) = hooks[i].address;
		unsigned highGroup = (debugRegisterSizeOne << 2) | debugRegisterTypeExecute;
		threadContext.Dr7 |= (highGroup << (4 * i + 16)) | (1 << (2 * i));
	}

	for(std::size_t i = hooks.size(); i < maximumHookCount; i++)
		*(debugRegisters[i]) = 0;
}

void processThreadIdentifier(DWORD threadIdentifier)
{
	std::string threadString = ail::hex_string_32(threadIdentifier);
	std::cout << "Processing thread " << threadString << std::endl;
	HANDLE threadHandle = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, threadIdentifier);
	if(threadHandle == NULL)
	{
		std::cout << "Unable to open thread " << threadString << " with appropriate permissions" << std::endl;
		return;
	}

	DWORD result = SuspendThread(threadHandle);
	if(result == -1)
	{
		std::cout << "Unable to suspend thread " << threadString << std::endl;
		return;
	}

	CONTEXT threadContext;
	threadContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	BOOL contextSuccess = GetThreadContext(threadHandle, &threadContext);
	if(!contextSuccess)
	{
		std::cout << "Unable to retrieve context of thread " << threadString << std::endl;
		return;
	}

	setDebugRegisters(threadContext);

	contextSuccess = SetThreadContext(threadHandle, &threadContext);
	if(!contextSuccess)
	{
		std::cout << "Unable to modify context of thread " << threadString << std::endl;
		return;
	}

	DWORD resumeResult = ResumeThread(threadHandle);
	if(resumeResult == -1)
	{
		std::cout << "Unable to resume execution of thread " << threadString << std::endl;
		return;
	}
}

void activateDebugRegisters()
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if(snapshot == INVALID_HANDLE_VALUE)
	{
		std::cout << "Unable to create thread snapshot" << std::endl;
		return;
	}
	DWORD currentThreadId = GetCurrentThreadId();
	THREADENTRY32 entry;
	entry.dwSize = static_cast<DWORD>(sizeof(THREADENTRY32));
	BOOL success = Thread32First(snapshot, &entry);
	DWORD currentProcess = GetCurrentProcessId();
	while(success)
	{
		if(entry.th32OwnerProcessID == currentProcess && entry.th32ThreadID != currentThreadId)
			processThreadIdentifier(entry.th32ThreadID);
		success = Thread32Next(snapshot, &entry);
	}
	CloseHandle(snapshot);
}

