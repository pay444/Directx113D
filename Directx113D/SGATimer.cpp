#include "stdafx.h"
#include "SGATimer.h"


SGATimer::SGATimer() : mdSecondsPerCount(0.0), 
						mdDeltaTime(-1.0),
						mfScale(1.f),
						mllBaseTime(0),
						mllPausedTime(0), 
						mllPrevTime(0), 
						mllCurrTime(0), 
						mbStopped(false)
{
	long long countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mdSecondsPerCount = 1.0 / (double)countsPerSec;
}

float SGATimer::TotalTime() const
{
	if (mbStopped)
	{
		return (float)(((mllStopTime - mllPausedTime) - mllBaseTime)*mdSecondsPerCount);
	} else
	{
		return (float)(((mllCurrTime - mllPausedTime) - mllBaseTime)*mdSecondsPerCount);
	}
}

float SGATimer::DeltaTime() const
{
	return (float)mdDeltaTime * mfScale;
}

void SGATimer::Start()
{
	long long currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mllBaseTime = currTime;
	mllPrevTime = currTime;
	mllStopTime = 0;
	mbStopped = false;
}

void SGATimer::Resume()
{
	long long startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mbStopped)
	{
		mllPausedTime += (startTime - mllStopTime);

		mllPrevTime = startTime;
		mllStopTime = 0;
		mbStopped = false;
	}
}

void SGATimer::Stop()
{
	if (!mbStopped)
	{
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mllStopTime = currTime;
		mbStopped = true;
	}
}

void SGATimer::Update()
{
	if (mbStopped)
	{
		mdDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mllCurrTime = currTime;

	mdDeltaTime = (mllCurrTime - mllPrevTime)*mdSecondsPerCount;

	mllPrevTime = mllCurrTime;

	if (mdDeltaTime < 0.0)
	{
		mdDeltaTime = 0.0;
	}
}

void SGATimer::SetScale(float scale)
{
	mfScale = scale;
}
