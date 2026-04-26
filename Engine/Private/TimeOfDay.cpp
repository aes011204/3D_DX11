#include "TimeOfDay.h"

CTimeOfDay::CTimeOfDay()
{
}

CTimeOfDay::~CTimeOfDay()
{
	Free();
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


	//IsNight = (m_fTOD01 >= 0.75f) || (m_fTOD01 <= 0.25f);
	IsNight = (m_fTOD01 >= m_fSunset01) || (m_fTOD01 < m_fSunrise01);
}

void CTimeOfDay::Set_TimeScale(_float timeScale)
{
	m_TimeScale = timeScale;
}

void CTimeOfDay::ComputeTime(_uint& iDay,_float& fHour, _float& fMinute, _float& fSecond)
{
	iDay = m_Day;
	_float fGameSecond = m_fTOD01 * 86400.f;

	fHour = fGameSecond / 3600.f;
	fMinute = fmod(fGameSecond, 3600.f) / 60.f;
	fSecond = fmod(fmod(fGameSecond, 3600.f) , 60.f);
}

void CTimeOfDay::Set_TOD01(_float tod)
{
	{
		m_fTOD01 = clamp(tod, 0.f, 1.f);
		m_Second = m_fTOD01 * m_SecondPerDay;
	}

}

void CTimeOfDay::OnGui()
{
	CBase::OnGui();
	ImGui::DragFloat("Time Scale", &m_TimeScale, 0.01f, 0.f, 100.f, "%.2f");

	ImGui::Separator(); // 구분선
	
	ImGui::Text("Current TOD: %.3f", m_fTOD01);

	ImGui::ProgressBar(m_fTOD01, ImVec2(0.f, 0.f), "Day/Night Progress");

	_uint iDay;
	_float fHour, fMin, fSec;
	// 작성하신 함수 호출
	ComputeTime(iDay, fHour, fMin, fSec);

	ImGui::TextColored(ImVec4(0.2f, 1.f, 0.5f, 1.f), "[ Current Status ]");

	// 날짜와 시:분:초 출력 (%02d는 두자리 고정, 앞자리를 0으로 채움)
	ImGui::Text("In-Game Date : Day %d", iDay);
	ImGui::Text("In-Game Time : %02d : %02d : %02d", (_int)fHour, (_int)fMin, (_int)fSec);


}

unique_ptr<CTimeOfDay> CTimeOfDay::Create()
{
	unique_ptr<CTimeOfDay> pInstance(new CTimeOfDay());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTimeOfDay");
	}
	return pInstance;
}

void CTimeOfDay::Free()
{
	__super::Free();
}
