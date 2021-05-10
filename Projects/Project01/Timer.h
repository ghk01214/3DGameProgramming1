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

	void Tick(FLOAT fLockFPS = 0.0f);
	void Start();
	void Stop();
	void Reset();

    ULONG GetFrameRate(LPTSTR lpszString = nullptr, INT nCharacters=0);
    FLOAT GetTimeElapsed();
	FLOAT GetTotalTime();

private:
	DOUBLE							m_fTimeScale;						
	FLOAT							m_fTimeElapsed;		

	INT64							m_nBasePerformanceCounter;
	INT64							m_nPausedPerformanceCounter;
	INT64							m_nStopPerformanceCounter;
	INT64							m_nCurrentPerformanceCounter;
    INT64							m_nLastPerformanceCounter;

	INT64							m_PerformanceFrequencyPerSec;				

    FLOAT							m_fFrameTime[MAX_SAMPLE_COUNT];
    ULONG							m_nSampleCount;

    ULONG					m_nCurrentFrameRate;				
	ULONG					m_FramePerSecond;					
	FLOAT							m_fFPSTimeElapsed;		

	BOOL							m_bStopped;
};
