#pragma once
//-----------------------------------------------------------------------------
// File: CGameTimer.h
//-----------------------------------------------------------------------------

const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();
public:
	void Tick(FLOAT fLockFPS = 0.0f);
	void Start();
	void Stop();
	void Reset();
public:
	ULONG GetFrameRate(LPTSTR lpszString = nullptr, INT nCharacters = 0);
	FLOAT GetTimeElapsed();
	FLOAT GetTotalTime();

private:
	DOUBLE							m_fTimeScale;
	FLOAT							m_fTimeElapsed;
private:
	INT64							m_nBasePerformanceCounter;
	INT64							m_nPausedPerformanceCounter;
	INT64							m_nStopPerformanceCounter;
	INT64							m_nCurrentPerformanceCounter;
	INT64							m_nLastPerformanceCounter;
private:
	INT64							m_PerformanceFrequencyPerSec;
private:
	FLOAT							m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG							m_nSampleCount;
private:
	ULONG							m_nCurrentFrameRate;
	ULONG							m_FramePerSecond;
	FLOAT							m_fFPSTimeElapsed;
private:
	BOOL							m_bStopped;
};
