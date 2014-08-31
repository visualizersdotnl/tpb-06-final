#pragma once

#include "stdafx.h"


// this will remove the filename (including extension) from a full filename
// so:
// c:\kip\koe\hoi.txt -> becomes -> c:\kip\koe\ 
// c:\jawel.txt -> becomes -> c:\ 
std::string RemoveFilenameFromPath(const std::string& filename);


// this will return the filename (with extension) from a full filename
// so:
// c:\kip\koe\hoi.txt -> becomes -> hoi.txt
std::string GetFilenameFromPath(const std::string& filename);

// this will return the filename (without extension) from a full filename
// so:
// c:\kip\koe\hoi.txt -> becomes -> hoi
std::string GetFilenameWithoutExtFromPath(const std::string& filename);

// binary-safe file content writing and reading routines

std::string ReadFileContents(const std::string& filename);
void WriteFileContents(const std::string& filename, const std::string& data);

void ReadFileContents(const std::string& filename, unsigned char** outBuffer, int* outBytesRead);

// find out if a file with specified filename exists.
bool FileExists(const std::string& filename);

// Get time a file was last modified, or -1 when unknown
__time64_t GetFileLastModifiedTime(const std::string& filename);

// Get the full filename for a temporary file
std::string GetTemporaryFileFileName();

// Get full filename of the current process' executable
std::string GetCurrentProcessFileName();

class FileChangeCheck
{
public:
	FileChangeCheck(const std::string& filename);

	bool IsFileDirty();

private:
	std::string filename;
	__time64_t lastModified;
};