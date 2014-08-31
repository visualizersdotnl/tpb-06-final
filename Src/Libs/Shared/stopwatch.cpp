#include "stdafx.h"
#include "stopwatch.h"

Stopwatch::Stopwatch()
{
	QueryPerformanceCounter(&initial);
	QueryPerformanceFrequency(&freq);
}

unsigned int Stopwatch::GetMillisecondsElapsed() const
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	current.QuadPart -= initial.QuadPart;
	current.QuadPart *= 1000;
	current.QuadPart /= freq.QuadPart;

	return (unsigned int)current.QuadPart;
}

unsigned int Stopwatch::GetMicrosecondsElapsed() const
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	current.QuadPart -= initial.QuadPart;
	current.QuadPart *= 1000000;
	current.QuadPart /= freq.QuadPart;

	return (unsigned int)current.QuadPart;
}

float Stopwatch::GetSecondsElapsed() const
{
	LARGE_INTEGER elapsed;
	QueryPerformanceCounter(&elapsed);

	elapsed.QuadPart -= initial.QuadPart;

	return (float)((double)elapsed.QuadPart / (double)freq.QuadPart);
}

LONGLONG Stopwatch::GetSamplesElapsed() const
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	return current.QuadPart - initial.QuadPart;
}

float Stopwatch::GetSecondsElapsedAndReset()
{
	LARGE_INTEGER current, elapsed;
	QueryPerformanceCounter(&current);

	elapsed.QuadPart = current.QuadPart - initial.QuadPart;
	initial.QuadPart = current.QuadPart;

	return (float)((double)elapsed.QuadPart / (double)freq.QuadPart);
}

int Stopwatch::GetSamplesElapsedAndReset(int samplesPerInterval, int extraOffset) 
{
	LARGE_INTEGER current, elapsed;
	QueryPerformanceCounter(&current);

	current.QuadPart += extraOffset;
	
	elapsed.QuadPart = current.QuadPart - initial.QuadPart;
	elapsed.QuadPart *= samplesPerInterval;
	elapsed.QuadPart /= freq.QuadPart;

	initial.QuadPart = current.QuadPart;

	return (int)elapsed.QuadPart;
}


void Stopwatch::Reset()
{
	QueryPerformanceCounter(&initial);
}

#ifdef _DEBUG 

StopwatchAccumulator::StopwatchAccumulator(int maxSamples)
	: numSamples(0), maxSamples(maxSamples), total(0)
{
	QueryPerformanceFrequency(&freq);
}

void StopwatchAccumulator::Add(const Stopwatch &stopwatch)
{
	if (++numSamples > maxSamples)
	{
		total = 0;
		numSamples = 0;
	}

	total += stopwatch.GetSamplesElapsed();
}


float StopwatchAccumulator::GetAverageTime() const
{
	return (float)((double)total / (double)(freq.QuadPart * numSamples));
}

bool StopwatchAccumulator::IsFull() const
{
	return (numSamples == maxSamples);
}

#endif