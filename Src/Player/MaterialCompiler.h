
#pragma once

class MaterialCompiler // : public NoCopy
{
public:
	MaterialCompiler();
	~MaterialCompiler();

	void StartJob(const unsigned char *source, int sourceLen, unsigned char **bytecode, int *bytecodeSize, std::string &errorMsg);
	void WaitForCompletion();

private:
	class Job
	{
	public:
		Job(std::string &errorMsg) : errorMsg(errorMsg) {}
		std::string &errorMsg;

		const unsigned char *source;
		int sourceLen;
		unsigned char **bytecode;
		int *bytecodeSize;
		bool ready;
	};

	std::vector<Job *> m_jobs;
	std::vector<HANDLE> m_threads;
	bool m_locked;

	static DWORD WINAPI ThreadProc(void *context);
};
