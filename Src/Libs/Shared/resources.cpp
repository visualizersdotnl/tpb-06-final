#include "stdafx.h"
#include "resources.h"

BinaryResourceReader::BinaryResourceReader(
	HINSTANCE hInstance,
	int id,
	const char* type)
{
	// @plek: If this crashes you shouldn't be shipping anyway.
	res = FindResource(hInstance, MAKEINTRESOURCE(id), type);
	resGlobal = LoadResource(hInstance, res);
	size = SizeofResource(hInstance, res);
	lockedBuffer = (unsigned char*)LockResource(resGlobal);
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
