#pragma once

#include <string>
#include <windows.h>

// MS defines GetMessage (to GetMessageA) which interferes with our poorly
// named GetMessage routine in Exception.
#undef GetMessage

class Exception
{
protected:
	std::string msg;

public:
	Exception(const std::string& msg)
		: msg(msg)
	{}
	virtual ~Exception()
	{}

	const std::string& GetMessage() const
	{
		return msg;
	}
};

class Win32Exception : public Exception
{
public:
	Win32Exception();
};


// this SystemException gets thrown by our SystemExceptionTranslator,
// in case we get a real naughty exception.
class SystemException : public Exception
{
public:
	SystemException()
		: Exception("System Exception")
	{}
};


void SystemExceptionTranslator( unsigned int u, EXCEPTION_POINTERS* pExp );



