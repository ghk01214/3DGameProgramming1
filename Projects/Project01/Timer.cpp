//-----------------------------------------------------------------------------
// File: CGameTimer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Timer.h"

CGameTimer::CGameTimer()
{
	::QueryPerformanceFrequency((LARGE_INTEGER *)&m_PerformanceFrequencyPerSec);
	::QueryPerformanceCounter((LARGE_INTEGER *)&m_nLastPerformanceCounter); 
	m_fTimeScale = 1.0 / (DOUBLE)m_PerformanceFrequencyPerSec;

	m_nBasePerformanceCounter = m_nLastPerformanceCounter;
	m_nPausedPerformanceCounter = 0;
	m_nStopPerformanceCounter = 0;

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_FramePerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick(FLOAT fLockFPS)
{
	if (m_bStopped)
	{
		m_fTimeElapsed = 0.0f;

		return;
	}

	FLOAT fTimeElapsed;

	::QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentPerformanceCounter);
	fTimeElapsed = FLOAT((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale);

    if (fLockFPS > 0.0f)
    {
        while (fTimeElapsed < (1.0f / fLockFPS))
        {
	        ::QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentPerformanceCounter);
	        fTimeElapsed = FLOAT((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale);
        }
    } 

	m_nLastPerformanceCounter = m_nCurrentPerformanceCounter;

    if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
    {
        ::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(FLOAT));

        m_fFrameTime[0] = fTimeElapsed;

        if (m_nSampleCount < MAX_SAMPLE_COUNT) 
			++m_nSampleCount;
    }

	++m_FramePerSecond;
	m_fFPSTimeElapsed += fTimeElapsed;

	if (m_fFPSTimeElapsed > 1.0f) 
    {
		m_nCurrentFrameRate	= m_FramePerSecond;
		m_FramePerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	} 

    m_fTimeElapsed = 0.0f;

	for (ULONG i = 0; i < m_nSampleCount; ++i)
	{
		m_fTimeElapsed += m_fFrameTime[i];
	}

    if (m_nSampleCount > 0)
		m_fTimeElapsed /= m_nSampleCount;
}

ULONG CGameTimer::GetFrameRate(LPTSTR lpszString, INT nCharacters) 
{
    if (lpszString)
    {
        _itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
        wcscat_s(lpszString, nCharacters, _T(" FPS)"));
    } 

    return m_nCurrentFrameRate;
}

FLOAT CGameTimer::GetTimeElapsed() 
{
    return m_fTimeElapsed;
}

FLOAT CGameTimer::GetTotalTime()
{
	if (m_bStopped)
		return FLOAT(((m_nStopPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * m_fTimeScale);

	return FLOAT(((m_nCurrentPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * m_fTimeScale);
}

void CGameTimer::Reset()
{
	INT64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	m_nBasePerformanceCounter = nPerformanceCounter;
	m_nLastPerformanceCounter = nPerformanceCounter;
	m_nStopPerformanceCounter = 0;
	m_bStopped = FALSE;
}

void CGameTimer::Start()
{
	INT64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER *)&nPerformanceCounter);

	if (m_bStopped)
	{
		m_nPausedPerformanceCounter += (nPerformanceCounter - m_nStopPerformanceCounter);
		m_nLastPerformanceCounter = nPerformanceCounter;
		m_nStopPerformanceCounter = 0;
		m_bStopped = FALSE;
	}
}

void CGameTimer::Stop()
{
	if (!m_bStopped)
	{
		::QueryPerformanceCounter((LARGE_INTEGER *)&m_nStopPerformanceCounter);
		m_bStopped = TRUE;
	}
}