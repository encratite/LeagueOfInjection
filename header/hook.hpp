#pragma once

#include <string>
#include <vector>

#include <Windows.h>

typedef void (*HookFunction)(CONTEXT &);

struct Hook
{
	std::string description;
	DWORD address;
	HookFunction handler;

	Hook(std::string const & description, DWORD address, HookFunction handler);
};

typedef std::vector<Hook> Hooks;

extern Hooks hooks;

void installHook(Hook const & hook);