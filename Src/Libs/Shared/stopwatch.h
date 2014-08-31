#pragma once

#include <windows.h>

class Stopwatch
{
private:
	LARGE_INTEGER initial, freq;

public:
	Stopwatch();

	void Reset();

	unsigned int GetMillisecondsElapsed() const;
	unsigned int GetMicrosecondsElapsed() const;
	float GetSecondsElapsed() const;
	float GetSecondsElapsedAndReset();
	LONGLONG GetSamplesElapsed() const;
	int GetSamplesElapsedAndReset(int samplesPerInterval, int extraOffset);
};

#ifdef _DEBUG

class StopwatchAccumulator
{
private:
	LONGLONG total;
	LARGE_INTEGER freq;

	int numSamples, maxSamples;

public:
	StopwatchAccumulator(int maxSamples);

	void Add(const Stopwatch& stopwatch);

	float GetAverageTime() const;

	bool IsFull() const;
};

#endif