#pragma once

#include "exception.h"

class MutexException
	: public Exception
{
public:
	enum MutexAction
	{
		ACTION_LOCK,
		ACTION_UNLOCK
	};

	MutexException(MutexAction action);

};


class Mutex
{
public:
	Mutex();
	~Mutex();

	void Lock();
	void Unlock();

private:
	HANDLE handle;
};


class MutexScopeLock
{
public:
	MutexScopeLock(Mutex& mutex);
	~MutexScopeLock();

private:
	// no assignment operator nor copy ctor
	MutexScopeLock(const MutexScopeLock&);
	MutexScopeLock& operator=(const MutexScopeLock&);

	Mutex& mutex;
};