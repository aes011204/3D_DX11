#include "CScaleModifier.h"
#include "UITransform.h"
#include "UI.h"

CScaleModifier::CScaleModifier(_float range, _float speed, _float startPosRad, _float baseScale, _bool continuous)
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

	_float sin = m_Range * sinf(m_Speed * m_fAccTime + m_StartPosRad) + m_BaseScale;
	m_pOwnerTransform->SetLocalScale({sin,sin});
}

void CScaleModifier::OnAppear(float fDeltaTime, CUI* pOwner)
{
}

void CScaleModifier::OnDisappear(float fDeltaTime, CUI* pOwner)
{
}
