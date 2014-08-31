#pragma once


class BinaryResourceReader
{
public:
	BinaryResourceReader(
		HINSTANCE hInstance,
		int id,
		const char* type);
	virtual ~BinaryResourceReader();

	int Size();
	unsigned char* GetBuffer();

private:
	HRSRC res;
	HGLOBAL resGlobal;
	
	int size;
	unsigned char* lockedBuffer;
};