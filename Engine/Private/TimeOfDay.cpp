#include "TimeOfDay.h"

CTimeOfDay::CTimeOfDay()
{
}

HRESULT CTimeOfDay::Initialize()
{
	return S_OK;
}

void CTimeOfDay::Update(_float fDeltaTime)
{
	m_Second += fDeltaTime*m_TimeScale;
	if(m_Second >= m_SecondPerDay)
	{
		m_Day++;
		m_Second = fmod(m_Second, m_SecondPerDay);
	}
	m_fTOD01 = m_Second / m_SecondPerDay;


	IsNight = (m_fTOD01 >= m_fSunset01) || (m_fTOD01 < m_fSunrise01);
	
}

void CTimeOfDay::Set_TimeScale(_float timeScale)
{
	m_TimeScale = timeScale;
}

void CTimeOfDay::ComputeTime(_uint& iDay,_float& fHour, _float& fMinute, _float& fSecond)
{
	iDay = m_Day;
	
	fHour = m_Second / 3600.f;
	fMinute = fmod(m_Second , 3600.f) / 60.f;
	fSecond = fmod(fmod(m_Second , 3600.f) , 60.f);
}

CTimeOfDay* CTimeOfDay::Create()
{
	CTimeOfDay* pInstance = new CTimeOfDay();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTimeOfDay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTimeOfDay::Free()
{
	__super::Free();
}
