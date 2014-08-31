#include "stdafx.h"
#include "mutex.h"

//
// MutexException
//
MutexException::MutexException(MutexAction action)
: Exception("")
{
	switch (action)
	{
	case ACTION_LOCK:
		msg = "Could not lock mutex.";
		break;
	case ACTION_UNLOCK:
		msg = "Could not unlock mutex.";
		break;
	default:
		msg = "Unknown mutex exception.";
	}
}


//
// Mutex
//
Mutex::Mutex()
{
	handle = CreateMutex(NULL, false, NULL);

	if (!handle)
		throw Win32Exception();
}

Mutex::~Mutex()
{
	CloseHandle(handle);
}

void Mutex::Lock()
{
	WaitForSingleObject(handle, INFINITE);
}


void Mutex::Unlock()
{
	ReleaseMutex(handle);
}


//
// MutexScopeLock
//
MutexScopeLock::MutexScopeLock(Mutex& mutex)
: mutex(mutex)
{
	mutex.Lock();
}

MutexScopeLock::~MutexScopeLock()
{
	mutex.Unlock();
}
