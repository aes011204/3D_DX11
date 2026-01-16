#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{ }


HRESULT CTimer_Manager::Add_Timer(const _wstring& timerTag)
{
    CTimer* timer = Find_Timer(timerTag);

    if (timer != nullptr)
        return E_FAIL;

    timer = CTimer::Create();

    if (nullptr == timer)
        return E_FAIL;

    m_Timers.emplace( timerTag, timer );

    return S_OK;
}

_float CTimer_Manager::Compute_TimeDelta(const _wstring& timerTag)
{
    CTimer* timer = Find_Timer(move(timerTag));

    if (nullptr == timer)
        return 0.f;

    return timer->Update_Timer();
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& timerTag)
{
  /*  auto iter
        = find_if(m_Timers.begin(), m_Timers.end(), CTag_Finder(move(timerTag)));*/
   
	// wstring 으로 바꿔서 이제find() 가능 이진탐색
    auto it = m_Timers.find(timerTag);

    if (it == m_Timers.end())
        return nullptr;

    return it->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
    return new CTimer_Manager;
}

void CTimer_Manager::Free()
{
    __super::Free();

    //for_each(m_Timers.begin(), m_Timers.end(), CDeleteMap());
    //m_Timers.clear();

    for (auto& Pair : m_Timers)
        Safe_Release(Pair.second); // 실제 안의 내용을 삭제 변경 해야 하므로 참조로 받아옴
    m_Timers.clear();
}

