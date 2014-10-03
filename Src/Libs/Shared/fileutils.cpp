#include "stdafx.h"
#include "fileutils.h"
#include <sys/stat.h>
#include "../Player/SetLastError.h"

const std::string RemoveFilenameFromPath(const std::string& filename)
{
	int index = (int)filename.find_last_of("\\/");

	// no slash found
	if (index == std::string::npos)
		return "";

	return filename.substr(0, index+1);
}

const std::string GetFilenameFromPath(const std::string& filename)
{
	int index = (int)filename.find_last_of("\\/");

	// no slash found
	if (index == std::string::npos)
		return filename;

	return filename.substr(index+1);
}

const std::string GetFilenameWithoutExtFromPath(const std::string& filename)
{
	std::string filenameWithExt = GetFilenameFromPath(filename);

	int index = (int)filenameWithExt.find_last_of(".");

	// no slash found
	if (index == std::string::npos)
		return filenameWithExt;

	return filenameWithExt.substr(0, index);
}

// binary-safe file content reading
bool ReadFileContents(const std::string& filename, std::string &res)
{
	FILE * f;
	if (fopen_s(&f, filename.c_str(), "rb") || (!f))
	{
		SetLastError("Could not read file: " + filename);
		return false;
	}

	int numRead = 0;
	
	do
	{
		char buf[4096];

		numRead = (int)fread(buf, 1, 4096, f);

		int currentSize = (int)res.size();
		res.resize(currentSize+numRead);

		// safe-copy read data (very very inefficient isn't it? :))
		// /me to lazy atm.
		for (int i=0; i<numRead; ++i)
			res[currentSize+i] = buf[i];

	} while (numRead == 4096);

	fclose(f);
	return true;
}


// binary-safe file content writing
bool WriteFileContents(const std::string& filename, const std::string& data)
{
	FILE * f;
	if (fopen_s(&f, filename.c_str(), "wb") || (!f))
	{
		SetLastError("Could not write file: " + filename);
		return false;
	}

	fwrite(data.c_str(), 1, data.length(), f);

	fclose(f);
	return true;
}

bool FileExists(const std::string& filename)
{
	FILE* f = NULL;

	fopen_s(&f, filename.c_str(), "rb");
	if (!f)
		return false;
	fclose(f);

	return true;
}

bool ReadFileContents(const std::string& filename, unsigned char** outBuffer, int* outBytesRead)
{
	FILE * f = NULL;
	if (fopen_s(&f, filename.c_str(), "rb") || (!f))
	{
		SetLastError("Could not read file: " + filename);
		return false;
	}

	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);	

	unsigned char* buffer = new unsigned char[filesize+1];
	buffer[filesize] = 0;

	*outBytesRead = (int)fread(buffer, 1, filesize, f);
	*outBuffer = buffer;

	fclose(f);
	return true;
}

__time64_t GetFileLastModifiedTime(const std::string& filename)
{
	struct __stat64 fileinfo;

	int result = _stat64(filename.c_str(), &fileinfo);
	if (result < 0)
		return -1;

	return fileinfo.st_mtime;
}

const std::string GetCurrentProcessFileName()
{
	char filename[2048];
	GetModuleFileName(NULL, filename, sizeof(filename));
	return std::string(filename);
}

FileChangeCheck::FileChangeCheck(const std::string& filename)
: filename(filename), lastModified(-1)
{
	lastModified = GetFileLastModifiedTime(filename);
}

bool FileChangeCheck::IsFileDirty()
{
	__time64_t stamp = GetFileLastModifiedTime(filename);

	if (stamp == lastModified)
	{
		return false;
	}
	else if (false == CanAccessFile())
	{
		// We can't access this file (yet?), so pretend it hasn't been changed atm.
		return false;
	}
	else
	{
		lastModified = stamp;
		return (stamp != -1);
	}
}


bool FileChangeCheck::CanAccessFile() const
{
	FILE* f = NULL;
	fopen_s(&f, filename.c_str(), "rb");
	if (f == NULL)
		return false;

	fclose(f);
	return true;
}
