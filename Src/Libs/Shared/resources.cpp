#include "stdafx.h"
#include "resources.h"
#include "exception.h"

BinaryResourceReader::BinaryResourceReader(
	HINSTANCE hInstance,
	int id,
	const char* type)
{
	res = FindResource(hInstance, MAKEINTRESOURCE(id), type);
	if (!res)
		throw Win32Exception();

	resGlobal = LoadResource(hInstance, res);
	if (!resGlobal)
		throw Win32Exception();

	size = SizeofResource(hInstance, res);

	lockedBuffer = (unsigned char*)LockResource(resGlobal);
	if (!lockedBuffer)
		throw Win32Exception();
}

BinaryResourceReader::~BinaryResourceReader()
{
	// from msdn:
	// It is not necessary to unlock resources because the system automatically 
	// deletes them when the process that created them terminates.
}

int BinaryResourceReader::Size()
{
	return size;
}

unsigned char* BinaryResourceReader::GetBuffer()
{
	return lockedBuffer;
}