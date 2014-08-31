#include "stdafx.h"
#include "shell.h"
#include "exception.h"

bool ExecuteProcessBlocking(
	const std::string& executablePath, 
	const std::string& args,
	std::string& output)
{
	SECURITY_ATTRIBUTES sattr;
	ZeroMemory(&sattr, sizeof(sattr));
//	FillSomeMemory(&sattr, sizeof(sattr), 0);
	sattr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sattr.bInheritHandle = TRUE; 
	sattr.lpSecurityDescriptor = NULL; 

	HANDLE hInR = NULL;
	HANDLE hInW = NULL;
	HANDLE hOutR = NULL;
	HANDLE hOutW = NULL;

	if (!CreatePipe(&hInR, &hInW, &sattr, 0))
		throw Win32Exception();
	if (!CreatePipe(&hOutR, &hOutW, &sattr, 0))
		throw Win32Exception();
	if (!SetHandleInformation(hOutR, HANDLE_FLAG_INHERIT, 0))
		throw Win32Exception();
	if (!SetHandleInformation(hInW, HANDLE_FLAG_INHERIT, 0))
		throw Win32Exception();

	STARTUPINFO startInfo;
	ZeroMemory(&startInfo, sizeof(startInfo));

	startInfo.cb = sizeof(STARTUPINFO); 
	startInfo.hStdError = hOutW;
	startInfo.hStdOutput = hOutW;
	startInfo.hStdInput = hInR;
	startInfo.dwFlags |= STARTF_USESTDHANDLES;

	PROCESS_INFORMATION procInfo;
	ZeroMemory(&procInfo, sizeof(procInfo));

	std::string cmdLine = executablePath + " " + args;

	BOOL success = CreateProcess(
		executablePath.c_str(),		// exe 
		(LPSTR)cmdLine.c_str(),		// command line 
		NULL,						// process security attributes 
		NULL,						// primary thread security attributes 
		TRUE,						// handles are inherited 
		CREATE_NO_WINDOW,           // creation flags 
		NULL,						// use parent's environment 
		NULL,						// use parent's current directory 
		&startInfo,					// STARTUPINFO pointer 
		&procInfo);					// receives PROCESS_INFORMATION 

	if (!success)
		throw Win32Exception();

	// Now wait for process!
	if (WaitForSingleObject(procInfo.hProcess, INFINITE) != 0)
		throw Exception("Error waiting for process to terminate.");

	DWORD errorCode = 0;
	if (!GetExitCodeProcess(procInfo.hProcess, &errorCode))
		throw Win32Exception();

	// Read text from pipe
	{
		DWORD dwRead; 
		CHAR chBuf[1024]; 

		while (true) 
		{ 
			BOOL res = ReadFile(hOutR, chBuf, 1024, &dwRead, NULL);
			if (!res || dwRead == 0) 
				break; 

			for (unsigned int i=0; i<dwRead; ++i)
				output += chBuf[i];

			if (dwRead < 1024)
				break;
		} 
	}

	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);
	CloseHandle(hOutR);
	CloseHandle(hOutW);
	CloseHandle(hInR);
	CloseHandle(hInW);

	return (errorCode == 0);
}