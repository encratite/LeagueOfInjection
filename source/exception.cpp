#include <iostream>

#include <Windows.h>

#include <boost/foreach.hpp>

#include <ail/string.hpp>

#include "hook.hpp"

void checkForHook(CONTEXT & threadContext)
{
	BOOST_FOREACH(Hook const & hook, hooks)
	{
		if(hook.address != threadContext.Eip)
			continue;
		//std::cout << "Breakpoint: " << hook.description << std::endl;
		hook.handler(threadContext);
		break;
	}
}

LONG WINAPI vectoredExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
	EXCEPTION_RECORD & exceptionRecord = *(ExceptionInfo->ExceptionRecord);
	CONTEXT & threadContext = *(ExceptionInfo->ContextRecord);

	//std::cout << "Exception type " << ail::hex_string_32(exceptionRecord.ExceptionCode) << " at " << ail::hex_string_32(threadContext.Eip) << " in thread " << ail::hex_string_32(GetCurrentThreadId()) << std::endl;

	switch(exceptionRecord.ExceptionCode)
	{
	case EXCEPTION_BREAKPOINT:
		std::cout << "Breakpoint" << std::endl;
		return EXCEPTION_CONTINUE_EXECUTION;

	case EXCEPTION_SINGLE_STEP:
		checkForHook(threadContext);
		return EXCEPTION_CONTINUE_EXECUTION;

	case DBG_PRINTEXCEPTION_C:
		std::cout << "Debug message" << std::endl;
		break;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void installExceptionHandler()
{
	if(!AddVectoredExceptionHandler(0, &vectoredExceptionHandler))
		std::cout << "Unable to add vectored exception handler" << std::endl;
}