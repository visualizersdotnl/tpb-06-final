#pragma once

#include "stdafx.h"

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

// Binary-safe file content writing and reading routines.
bool ReadFileContents(const std::string& filenamem, std::string &res);
bool WriteFileContents(const std::string& filename, const std::string& data);
bool ReadFileContents(const std::string& filename, unsigned char** outBuffer, int* outBytesRead);

// Find out if a file with specified filename exists.
bool FileExists(const std::string& filename);

// Get time a file was last modified, or -1 when unknown.
__time64_t GetFileLastModifiedTime(const std::string& filename);

// Get full filename of the current process' executable.
const std::string GetCurrentProcessFileName();

// Dirty check for whatever sort of reloadin'.
class FileChangeCheck
{
public:
	FileChangeCheck(const std::string& filename);

	bool IsFileDirty();

private:
	bool CanAccessFile() const;

	std::string filename;
	__time64_t lastModified;
};
