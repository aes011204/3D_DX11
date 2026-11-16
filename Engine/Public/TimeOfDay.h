#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CTimeOfDay :
    public CBase
{
private:
    CTimeOfDay();
public:
    virtual ~CTimeOfDay()override;

public:
    HRESULT Initialize();
    void Update(_float deltaTime);

    void Set_TimeScale(_float timeScale);

    void ComputeTime(_uint& day, _float& hour, _float& minute, _float& second);

    _float Get_TOD01() { return m_TOD01; }

    void Set_TOD01(_float tod);
    _bool Get_IsNight() { return m_isNight; };
   
    void OnGui() override;
private:
    _float m_secondPerDay = 1200; // 스케일이 1 일떄 게임의 하루는 현실 20분
    _float m_timeScale = { 1.f }; // 1 정상 0.5 절반 느림 2 두배 빠름

    _uint m_day = { 0 };

    _float m_second = { 0 }; // 초기준으로 저장 시간 필요할떄 계산


    _float m_TOD01 = 0.f; // 0~1
    _float m_sunrise01 = 0.30f; // 원래는 0.25
    _float m_sunset01 = 0.75f;

    _bool m_isNight = { false };

public:
    static unique_ptr<CTimeOfDay> Create();
    void Free()override;


};

NS_END