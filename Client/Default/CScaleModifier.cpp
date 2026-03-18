#include "CScaleModifier.h"
#include "UITransform.h"
#include "UI.h"
#include "Log_Manager.h"

CScaleModifier::CScaleModifier(_float range, _float speed, _float startPosRad, _float2 baseScale, _bool continuous)
	: m_Range (range),
	m_Speed(speed),m_StartPosRad(startPosRad),
	m_BaseScale (baseScale),m_Continuous (continuous)
{

}
CScaleModifier::~CScaleModifier()
{
}

void CScaleModifier::Tick(float fDeltaTime, CUI* pOwner)
{
	if (nullptr == m_pOwnerTransform) 
	{
		m_pOwnerTransform = dynamic_cast<CUITransform*>(pOwner->Get_Component(g_strUITransformTag).get());
	}

	m_fAccTime += fDeltaTime;

	Vector2 size = {};

	_float sinX = m_Range * sinf(m_Speed * m_fAccTime + m_StartPosRad) + m_BaseScale.x;
	_float sinY = m_Range * sinf(m_Speed * m_fAccTime + m_StartPosRad) + m_BaseScale.y;
	m_pOwnerTransform->SetLocalScale({ sinX,sinY });

	//CLog_Manager::GetInstance()->Add_Log_F(LOG_LEVEL::INFO,"dt: %.4f  acc: %.3f  scale: %.4f", fDeltaTime, m_fAccTime, sin);
}

void CScaleModifier::OnAppear(float fDeltaTime, CUI* pOwner)
{
}

void CScaleModifier::OnDisappear(float fDeltaTime, CUI* pOwner)
{

}
