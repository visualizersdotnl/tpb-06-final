#include "stdafx.h"
#include "pipe.h"
#include "exception.h"

// construct a new pipe
Pipe::Pipe()
{
	handle = 0;
}

// destroy pipe
Pipe::~Pipe()
{
	CloseHandle(handle); 
}

// write data to a pipe
void Pipe::Write(char* buf, int numBytes)
{
	DWORD numWritten = 0;

	BOOL success = WriteFile( 
		handle,			// pipe handle 
		buf,			// message 
		numBytes,		// message length 
		&numWritten,    // bytes written 
		NULL);          // not overlapped 

	if (!success)
	{
		DWORD err = GetLastError();

		if (err == ERROR_BROKEN_PIPE)
			throw BrokenPipeException();
	}

	if (numWritten != numBytes)
		throw Exception("Error writing the specified number of bytes to the named pipe.");
}



void GetLastErrorMsg(char * msg)
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	sprintf(msg, "(%d) %s", dw, (char*)lpMsgBuf);

	LocalFree(lpMsgBuf);
}

// read data from a pipe (blocking)
void Pipe::Read(char* buf, int numBytes)
{
	DWORD totalNumRead = 0;
	char * dest = buf;

//	char msg[512];

	while (true)
	{
		DWORD numRead = 0;

		// do a read, which is blocking!
		BOOL success =			
			ReadFile( 
				handle,    // pipe handle 
				dest,    // buffer to receive reply 
				numBytes-totalNumRead,  // size of buffer 
				&numRead,  // number of bytes read 
				NULL);    // not overlapped 

		if (!success)
		{
			DWORD err = GetLastError();

			if (err == ERROR_BROKEN_PIPE)
				throw BrokenPipeException();
		}
//		GetLastErrorMsg(msg);
//		printf("Read returned: %d, numRead: %d/%d bytes. Last error: %s\n", (int)success, numRead, (numBytes-totalNumRead), msg);

		totalNumRead += numRead;
		dest += numRead;

		if (totalNumRead >= (DWORD)numBytes)
			return; // finished!
	}
}



// write a message
void Pipe::Write(const Pipe::Message& message)
{
	Write((char*)&message.size, sizeof(message.size));
	Write(message.data, message.size);
}


// read a message
Pipe::Message Pipe::Read()
{
	int size = 0;

	// read byte count
	Read((char*)&size, sizeof(size));

	Message msg(size);

	// read bytes
	Read(msg.data, msg.size);

	// copy bytes to output
	return msg;
}

Pipe::Message::Message(int size)
: size(size), data(new char[size])
{
}

Pipe::Message::Message(const Message& other)
{
	size = other.size;
	data = new char[other.size];
	memcpy(data, other.data, size);
}

Pipe::Message::~Message()
{
	delete [] data;
}



// create a client pipe
ClientPipe::ClientPipe(const std::string& name)
{
	handle = CreateFile( 
		name.c_str(),   // pipe name 
		GENERIC_READ |  // read and write access 
		GENERIC_WRITE, 
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 

	if (handle == INVALID_HANDLE_VALUE) 
	{
		// Exit if an error other than ERROR_PIPE_BUSY occurs.
		if (GetLastError() != ERROR_PIPE_BUSY) 
			throw Win32Exception();
		else
			throw Exception("Could not open pipe, pipe is busy!");
	}

	DWORD mode = PIPE_READMODE_BYTE; 
	BOOL success = SetNamedPipeHandleState( 
		handle,    // pipe handle 
		&mode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 

	if (!success)
		throw Exception("Error setting handlestate for pipe!");
}


ServerPipe::ConnectedPipe::ConnectedPipe(HANDLE handle)
{
	this->handle = handle;
}

ServerPipe::ConnectedPipe::~ConnectedPipe()
{
	FlushFileBuffers(handle); 
	DisconnectNamedPipe(handle); 
	CloseHandle(handle); 
}

ServerPipe::ServerPipe(const std::string& name)
{
	this->name = name;
}

ServerPipe::~ServerPipe()
{

}

#define BUFSIZE 4096

// listen to clients and handle a connection
ServerPipe::ConnectedPipe* ServerPipe::Listen()
{
	HANDLE handle = CreateNamedPipe( 
		name.c_str(),					// pipe name 
		PIPE_ACCESS_DUPLEX,       // read/write access 
		PIPE_TYPE_MESSAGE |       // message type pipe 
		PIPE_READMODE_BYTE |	  // byte-read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		BUFSIZE,                  // output buffer size 
		BUFSIZE,                  // input buffer size 
		NMPWAIT_USE_DEFAULT_WAIT, // client time-out 
		NULL);                    // default security attribute 

	if (handle == INVALID_HANDLE_VALUE)
		throw Exception("Error creating ServerPipe!");

	// Wait for the client to connect; if it succeeds, 
	// the function returns a nonzero value. If the function
	// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

	BOOL connected = ConnectNamedPipe(handle, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

	if (connected) 
		return new ConnectedPipe(handle);
	else
	{
		CloseHandle(handle);
		throw Exception("Nothing connected...");
	}

// 		DWORD threadId;
// 
// 		ConnectedPipe * pipe = new ConnectedPipe(handle);
// 
// 		// Create a thread for this client. 
// 		HANDLE threadHandle = CreateThread( 
// 			NULL,              // no security attribute 
// 			0,                 // default stack size 
// 			InstanceThread,    // thread proc
// 			(LPVOID) pipe,     // thread parameter 
// 			0,                 // not suspended 
// 			&threadId);      // returns thread ID 
// 
// 		if (threadHandle == NULL) 
// 		{
// 			delete pipe;
// 			throw Exception("Error spawning message handler thread for serverpipe.");
// 		}
// 		else 
// 			CloseHandle(threadHandle); 
}

// DWORD WINAPI ServerPipe::InstanceThread(LPVOID lpvParam) 
// { 
// 	ConnectedPipe * pipe = (ConnectedPipe*) lpvParam;
// 
// 	while (true)
// 	{
// 		// wait for message
// 		Pipe::Message msg = pipe->Read();
// 
// 		printf("Received: %s (%d bytes)\n", msg.data, msg.size);
// 
// 		// send a reply
// 		Pipe::Message reply(100);
// 		strcpy(reply.data, "This is a server reply!");
// 		pipe->Write(reply);
// 	}
// 
// 	delete pipe;
// 
// 	return 1;
// }