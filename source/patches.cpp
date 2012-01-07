#include <iostream>

#include <Windows.h>

#include "hotPatch.hpp"
#include "hook.hpp"

typedef HANDLE (WINAPI * CreateThreadType)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);

namespace
{
	CreateThreadType originalCreateThread;
}

HANDLE WINAPI patchedCreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	DWORD threadId;
	dwCreationFlags |= CREATE_SUSPENDED;
	HANDLE output = originalCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, &threadId);
	if(lpThreadId)
		*lpThreadId = threadId;

	replaceThreadContext(threadId, false);

	return output;
}

void installHotPatches()
{
	hotPatchFunction("kernel32.dll", "CreateThread", &patchedCreateThread, reinterpret_cast<void * &>(originalCreateThread));
}