#include "CScaleModifier.h"
#include "UITransform.h"
#include "UI.h"
#include "Log_Manager.h"

CScaleModifier::CScaleModifier(_float range, _float duration, _float startPosRad, _float2 baseScale, _bool continuous)
	: m_range (range),
	m_startPosRad(startPosRad),
	m_baseScale (baseScale),m_isContinuous (continuous),
	m_duration(duration)
{

}
CScaleModifier::~CScaleModifier()
{

	m_accTime = 0.f;
}

void CScaleModifier::Tick(float deltaTime, CUI* owner)
{
	if (nullptr == m_ownerTransform) 
	{
		m_ownerTransform = dynamic_cast<CUITransform*>(owner->Get_Component(g_strUITransformTag).get());
	}

	m_accTime += deltaTime;



	if (m_isContinuous == true)
	{
		// 이건 듀레이션 을 스피드로
		float curSin = sinf(m_duration * m_accTime + m_startPosRad);
		float sinValue = sinf(curSin * PI / 2.0f);

		Vector2 size = {};

		_float sinX = m_range * sinValue + m_baseScale.x;
		_float sinY = m_range * sinValue + m_baseScale.y;
		m_ownerTransform->SetLocalScale({ sinX,sinY });


	}

	if(m_isContinuous == false)
	{

		float ratio = m_accTime / m_duration;
		if (ratio >= 1.0f) ratio = 1.0f;

		//float CurSin = sinf(m_Speed * m_fAccTime + m_StartPosRad);
		float sinValue = sinf(ratio * PI / 2.0f);

		Vector2 size = {};

		_float sinX = m_range * sinValue + m_baseScale.x;
		_float sinY = m_range * sinValue + m_baseScale.y;
		m_ownerTransform->SetLocalScale({ sinX,sinY });


		if (ratio >= 1.0f)
		{
			m_finsh = true;

		}
	}
	//CLog_Manager::GetInstance()->Add_Log_F(LOG_LEVEL::INFO,"dt: %.4f  acc: %.3f  scale: %.4f", fDeltaTime, m_fAccTime, sin);
}

void CScaleModifier::OnAppear(float deltaTime, CUI* owner)
{
}

void CScaleModifier::OnDisappear(float deltaTime, CUI* owner)
{

}
