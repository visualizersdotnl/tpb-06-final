#include "stdafx.h"
#include "exception.h"


Win32Exception::Win32Exception()
: Exception("")
{
	// use showlasterror

	LPVOID msgbuf;
	SYSTEMTIME st;
	DWORD err = GetLastError();

	// Request local systemtime
	GetLocalTime(&st);

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &msgbuf,
		0,
		NULL 
		);

	msg = std::string((char *) msgbuf);
//	fprintf(f, "(%i:%i:%i.%i) at %s::%s (Message from GetLastError()): %s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
//		classname,methodname,(LPCSTR) msgbuf);

	// Free the buffer.
	LocalFree( msgbuf );
}



void SystemExceptionTranslator( unsigned int u, EXCEPTION_POINTERS* pExp )
{
    throw SystemException();
}
