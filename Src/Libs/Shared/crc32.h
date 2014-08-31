#pragma once

// CRC32 helper class, 
// kindly inspired on sourcecode from:
// http://www.gamedev.net/reference/programming/features/crc32/
class CRC32 
{
public:
	static unsigned int Calc( const char* s );

private:
	static inline void UpdateCRC32( const unsigned char byte, unsigned int& crc32 );

	static unsigned int table[256];
};