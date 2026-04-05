#include "CScaleModifier.h"
#include "UITransform.h"
#include "UI.h"
#include "Log_Manager.h"

CScaleModifier::CScaleModifier(_float range, _float duration, _float startPosRad, _float2 baseScale, _bool continuous)
	: m_Range (range),
	m_StartPosRad(startPosRad),
	m_BaseScale (baseScale),m_Continuous (continuous),
	m_Duration(duration)
{

}
CScaleModifier::~CScaleModifier()
{

	m_fAccTime = 0.f;
}

void CScaleModifier::Tick(float fDeltaTime, CUI* pOwner)
{
	if (nullptr == m_pOwnerTransform) 
	{
		m_pOwnerTransform = dynamic_cast<CUITransform*>(pOwner->Get_Component(g_strUITransformTag).get());
	}

	m_fAccTime += fDeltaTime;



	if (m_Continuous == true)
	{
		// 이건 듀레이션 을 스피드로
		float CurSin = sinf(m_Duration * m_fAccTime + m_StartPosRad);
		float sinValue = sinf(CurSin * PI / 2.0f);

		Vector2 size = {};

		_float sinX = m_Range * sinValue + m_BaseScale.x;
		_float sinY = m_Range * sinValue + m_BaseScale.y;
		m_pOwnerTransform->SetLocalScale({ sinX,sinY });


	}

	if(m_Continuous == false)
	{

		float fRatio = m_fAccTime / m_Duration;
		if (fRatio >= 1.0f) fRatio = 1.0f;

		//float CurSin = sinf(m_Speed * m_fAccTime + m_StartPosRad);
		float sinValue = sinf(fRatio * PI / 2.0f);

		Vector2 size = {};

		_float sinX = m_Range * sinValue + m_BaseScale.x;
		_float sinY = m_Range * sinValue + m_BaseScale.y;
		m_pOwnerTransform->SetLocalScale({ sinX,sinY });


		if (fRatio >= 1.0f)
		{
			m_finsh = true;

		}
	}
	//CLog_Manager::GetInstance()->Add_Log_F(LOG_LEVEL::INFO,"dt: %.4f  acc: %.3f  scale: %.4f", fDeltaTime, m_fAccTime, sin);
}

void CScaleModifier::OnAppear(float fDeltaTime, CUI* pOwner)
{
}

void CScaleModifier::OnDisappear(float fDeltaTime, CUI* pOwner)
{

}
