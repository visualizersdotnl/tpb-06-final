
#include <Core/Platform.h>
#include <Core/Core.h>
#include "MaterialCompiler.h"

MaterialCompiler::MaterialCompiler() :
		m_locked(false)
{
}

MaterialCompiler::~MaterialCompiler()
{
	for (auto pJob : m_jobs)
		delete pJob;

	for (auto hThread : m_threads)
		CloseHandle(hThread);
}

void MaterialCompiler::StartJob(
	const std::string &path,
	const unsigned char *source, int sourceLen, 
	unsigned char **bytecode, int *bytecodeSize, 
	std::string &errorMsg)
{
	if (false == m_locked)
	{
		// Spawn new job thread.
		Job *pJob = new Job(errorMsg);
		pJob->path = path;
		pJob->source = source;
		pJob->sourceLen = sourceLen;
		pJob->bytecode = bytecode;
		pJob->bytecodeSize = bytecodeSize;
		m_jobs.push_back(pJob);
		m_threads.push_back(CreateThread(NULL, 0, ThreadProc, pJob, 0, NULL));
	}
	else
	{
		ASSERT_MSG(0, "MaterialCompiler::StartJob() -> WaitForCompletion() already called!")
	}
}

void MaterialCompiler::WaitForCompletion()
{
	// For now this class is just for single-cycle use at startup.
	m_locked = true;

	while (true && 0 != m_threads.size())
	{
		// Wait for all threads to complete.
		if (WAIT_TIMEOUT != WaitForMultipleObjects((DWORD) m_threads.size(), &m_threads[0], TRUE, 1000))
			break;
	}
}

/* static */ DWORD WINAPI MaterialCompiler::ThreadProc(void *context)
{
	Job *pJob = reinterpret_cast<Job *>(context);

	// Compile FX...
	if (false == Pimp::gD3D->CompileEffect(
		pJob->path,
		pJob->source,
		pJob->sourceLen,
		pJob->bytecode,
		pJob->bytecodeSize,
		pJob->errorMsg))
	{
		// Quick way to indicate that compile has failed.
		*pJob->bytecodeSize = -1; 
	}

	// Done.
	pJob->ready = true;
	return 1;
}
