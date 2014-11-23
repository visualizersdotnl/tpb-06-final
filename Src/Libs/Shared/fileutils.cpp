
#include <Windows.h>
#include <sys/stat.h>
#include "fileutils.h"

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

bool FileExists(const std::string& filename)
{
	FILE* f = NULL;

	fopen_s(&f, filename.c_str(), "rb");
	if (!f)
		return false;
	fclose(f);

	return true;
}

bool ReadFileContent(const std::string& filename, unsigned char** ppDest, int* pBytesRead)
{
	FILE * f = NULL;
	if (fopen_s(&f, filename.c_str(), "rb") || (!f))
	{
		return false;
	}

	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);	

	unsigned char* buffer = new unsigned char[filesize+1];
	buffer[filesize] = 0;

	*pBytesRead = (int)fread(buffer, 1, filesize, f);
	*ppDest = buffer;

	fclose(f);
	return true;
}

bool WriteFileContent(const std::string& filename, unsigned char* pSrc, int bytesToWrite)
{
	FILE * f;
	if (fopen_s(&f, filename.c_str(), "wb") || (!f))
	{
		return false;
	}

	fwrite(pSrc, 1, bytesToWrite, f);

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

bool FileChangeCheck::HasChanged()
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
