
#pragma once

class MaterialCompiler // : public NoCopy
{
public:
	MaterialCompiler();
	~MaterialCompiler();

	void StartJob(const unsigned char *source, int sourceLen, unsigned char **bytecode, int *bytecodeSize);
	void WaitForCompletion();

private:
	struct Job
	{
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
