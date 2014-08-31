#pragma once

//////////////////////////////////////////////////////////////////////////
// info taken from http://msdn2.microsoft.com/en-us/library/aa365592.aspx
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <string>
#include <vector>
#include "exception.h"


// a named pipe
class Pipe
{
protected:
	HANDLE handle;

	Pipe();
	virtual ~Pipe();

	void Write(char* buf, int numBytes);
	void Read(char* buf, int numBytes);	

public:

	struct Message
	{
	public:
		int size;
		char* data;
	
		Message(int size);
		Message(const Message& other);
		~Message();
	};

	void Write(const Message& message);
	Message Read();
};


// a client named pipe
class ClientPipe : public Pipe
{
public:
	ClientPipe(const std::string& name);
};



// a server named pipe
class ServerPipe
{
private:
	std::string name;

	static DWORD WINAPI InstanceThread(LPVOID lpvParam);

public:
	class ConnectedPipe : public Pipe
	{
	public:
		ConnectedPipe(HANDLE handle);
		~ConnectedPipe();
	};

	ServerPipe(const std::string& name);
	~ServerPipe();

	ConnectedPipe* Listen();
};


class BrokenPipeException : public Exception
{
public:
	BrokenPipeException()
		: Exception("Broken pipe!")
	{
	}
};

