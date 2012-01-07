#include "hook.hpp"

Hooks hooks;

Hook::Hook(std::string const & description, DWORD address, HookFunction handler):
	description(description),
	address(address),
	handler(handler)
{
}

void installHook(Hook const & hook)
{
	hooks.push_back(hook);
}