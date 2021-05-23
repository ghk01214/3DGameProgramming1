#pragma once

const ULONG MAX_COUNT{ 50 };

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();
public:
	void Start();
	void Stop();
	void Reset();
	void Tick(FLOAT fLockFPS = 0.0f);
public:
	ULONG GetFrameRate(LPTSTR lpszString = nullptr, INT nCharacters = 0);
	FLOAT GetTimeElapsed();
private:
	BOOL				m_bHardwareHasPerformanceCounter;
	FLOAT				m_fTimeScale;
	FLOAT				m_fTimeElapsed;
	INT64				m_nCurrentTime;
	INT64				m_nLastTime;
	INT64				m_nPerformanceFrequency;
private:
	FLOAT				m_fFrameTime[MAX_COUNT];
	ULONG				m_nSampleCount;
private:
	ULONG				m_nCurrentFrameRate;
	ULONG				m_nFramesPerSecond;
	FLOAT				m_fFPSTimeElapsed;
private:
	BOOL				m_bStopped;
};

