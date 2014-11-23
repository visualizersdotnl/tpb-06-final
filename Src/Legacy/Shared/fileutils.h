
#pragma once

#include <string>

// this will remove the filename (including extension) from a full filename
// so:
// c:\kip\koe\hoi.txt -> becomes -> c:\kip\koe\ 
// c:\jawel.txt -> becomes -> c:\ 
const std::string RemoveFilenameFromPath(const std::string& filename);

// this will return the filename (with extension) from a full filename
// so:
// c:\kip\koe\hoi.txt -> becomes -> hoi.txt
const std::string GetFilenameFromPath(const std::string& filename);

// this will return the filename (without extension) from a full filename
// so:
// c:\kip\koe\hoi.txt -> becomes -> hoi
const std::string GetFilenameWithoutExtFromPath(const std::string& filename);

// Binary-safe file read & write.
bool ReadFileContent(const std::string& filename, unsigned char** ppDest, int* pBytesRead);
bool WriteFileContent(const std::string& filename, unsigned char* pSrc, int bytesToWrite);

// Find out if a file with specified filename exists.
bool FileExists(const std::string& filename);

// Get time a file was last modified, or -1 when unknown.
__time64_t GetFileLastModifiedTime(const std::string& filename);

// Get full filename of the current process' executable.
const std::string GetCurrentProcessFileName();

// File change check.
class FileChangeCheck
{
public:
	FileChangeCheck(const std::string& filename);

	bool HasChanged();

private:
	bool CanAccessFile() const;

	std::string filename;
	__time64_t lastModified;
};
