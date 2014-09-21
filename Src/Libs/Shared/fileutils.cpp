#include "stdafx.h"
#include "fileutils.h"
#include "exception.h"
#include <sys/stat.h>


std::string RemoveFilenameFromPath(const std::string& filename)
{
	int index = (int)filename.find_last_of("\\/");

	// no slash found
	if (index == std::string::npos)
		return "";

	return filename.substr(0, index+1);
}


std::string GetFilenameFromPath(const std::string& filename)
{
	int index = (int)filename.find_last_of("\\/");

	// no slash found
	if (index == std::string::npos)
		return filename;

	return filename.substr(index+1);
}

std::string GetFilenameWithoutExtFromPath(const std::string& filename)
{
	std::string filenameWithExt = GetFilenameFromPath(filename);

	int index = (int)filenameWithExt.find_last_of(".");

	// no slash found
	if (index == std::string::npos)
		return filenameWithExt;

	return filenameWithExt.substr(0, index);
}



// binary-safe file content reading
std::string ReadFileContents(const std::string& filename)
{
	FILE * f;
	if (fopen_s(&f, filename.c_str(), "rb") || (!f))
		throw Exception("Could not read file "+filename);

	std::string res;

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

	return res;
}


// binary-safe file content writing
void WriteFileContents(const std::string& filename, const std::string& data)
{
	FILE * f;
	if (fopen_s(&f, filename.c_str(), "wb") || (!f))
		throw Exception("Could not write file "+filename);

	fwrite(data.c_str(), 1, data.length(), f);

	fclose(f);
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


void ReadFileContents(const std::string& filename, unsigned char** outBuffer, int* outBytesRead)
{
	FILE * f = NULL;
	if (fopen_s(&f, filename.c_str(), "rb") || (!f))
		throw Exception("Could not read file "+filename);

	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);	

	unsigned char* buffer = new unsigned char[filesize+1];
	buffer[filesize] = 0;

	*outBytesRead = (int)fread(buffer, 1, filesize, f);
	*outBuffer = buffer;

	fclose(f);
}


std::string GetTemporaryFileFileName()
{
	char tempPath[512];
	if (GetTempPath(sizeof(tempPath), tempPath) == 0)
		throw Win32Exception();
	
	char buffer[MAX_PATH+1];
	if (GetTempFileName(tempPath, "inq", 0, buffer) == 0)
		throw Win32Exception();

	return std::string(buffer);
}


__time64_t GetFileLastModifiedTime(const std::string& filename)
{
	struct __stat64 fileinfo;

	int result = _stat64(filename.c_str(), &fileinfo);
	if (result < 0)
		return -1;

	return fileinfo.st_mtime;
}


std::string GetCurrentProcessFileName()
{
	char filename[2048];

	DWORD result = GetModuleFileName(NULL, filename, sizeof(filename));

	if (result == 0)
		throw Win32Exception();

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
	else if (!CanAccessFile())
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

