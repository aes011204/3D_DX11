#include "Timer.h"

CTimer::CTimer()
    : m_timeDelta(0.f)
{
    ZeroMemory(&m_fixTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_lastTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_frameTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&m_cpuTick, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Ready_Timer()
{
    QueryPerformanceCounter(&m_frameTime);  // ex) 1077
    QueryPerformanceCounter(&m_lastTime);   // ex) 1085
    QueryPerformanceCounter(&m_fixTime);	// ex) 1090

    QueryPerformanceFrequency(&m_cpuTick);	// CPU Tick 값을 얻어오는 함수

    return S_OK;
}

_float CTimer::Update_Timer()
{
    QueryPerformanceCounter(&m_frameTime);  // ex) 1500

    if (m_frameTime.QuadPart - m_fixTime.QuadPart >= m_cpuTick.QuadPart)
    {
        QueryPerformanceFrequency(&m_cpuTick);
        m_fixTime = m_frameTime;
    }

    m_timeDelta = static_cast<_float>(m_frameTime.QuadPart - m_lastTime.QuadPart)
        / static_cast<_float>(m_cpuTick.QuadPart);

    m_lastTime = m_frameTime;

    return m_timeDelta;
}

shared_ptr<CTimer> CTimer::Create()
{
    shared_ptr<CTimer> pInstance(new CTimer());

    if (FAILED(pInstance->Ready_Timer()))
    {
        MSG_BOX("Timer Create Failed");

        return nullptr;
    }

    return pInstance;
}

void CTimer::Free()
{
    __super::Free();
}
