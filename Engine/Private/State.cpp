#include "State.h"

CState::CState() :m_curStateKey(0), m_isStarted(false), m_isExited(false), m_canTransit(false)
{
}

CState::~CState()
{
}

void CState::Enter()
{
    m_isStarted = true;
    m_isExited = false;
    m_canTransit = false;

    m_curStateElapsedTime = 0.f;
    m_isMinStateTimePassed = false;
}

void CState::Exit()
{
    m_isStarted = false;
    m_isExited = true;
}
