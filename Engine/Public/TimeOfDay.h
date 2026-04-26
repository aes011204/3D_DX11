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
    void Update(_float fDeltaTime);

    void Set_TimeScale(_float timeScale);

    void ComputeTime(_uint& iDay, _float& fHour, _float& fMinute, _float& fSecond);

    _float Get_TOD01() { return m_fTOD01; }

    void Set_TOD01(_float tod);
    _bool Get_IsNight() { return IsNight; };
   
    void OnGui() override;
private:
    _float m_SecondPerDay = 1200; // 스케일이 1 일떄 게임의 하루는 현실 20분
    _float m_TimeScale = { 1.f }; // 1 정상 0.5 절반 느림 2 두배 빠름

    _uint m_Day = { 0 };

    _float m_Second = { 0 }; // 초기준으로 저장 시간 필요할떄 계산


    _float m_fTOD01 = 0.f; // 0~1
    _float m_fSunrise01 = 0.30f; // 원래는 0.25
    _float m_fSunset01 = 0.75f;

    _bool IsNight = { false };

public:
    static unique_ptr<CTimeOfDay> Create();
    void Free()override;


};

NS_END