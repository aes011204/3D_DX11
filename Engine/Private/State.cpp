#include "State.h"

CState::CState() :m_dwCurStateKey(0), m_bStarted(false), m_bExited(false), m_bCanTransit(false)
{
}

CState::~CState()
{
}

void CState::Enter()
{
    m_bStarted = true;
    m_bExited = false;
    m_bCanTransit = false;

    m_fCurStateElapsedTime = 0.f;
    m_bMinStateTimePassed = false;
}

void CState::Exit()
{
    m_bStarted = false;
    m_bExited = true;
}
