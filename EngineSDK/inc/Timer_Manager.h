#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CTimer;

class CTimer_Manager : public CBase
{
private:
    explicit CTimer_Manager();
    virtual ~CTimer_Manager() = default;

public:
    _float Compute_TimeDelta(const _wstring& timerTag);

    HRESULT Add_Timer(const _wstring& timerTag);

private:
    map<_wstring, CTimer*> m_Timers;
private:
    CTimer* Find_Timer(const _wstring& timerTag);


public:
    static CTimer_Manager* Create();
    void Free() override;

};

NS_END
