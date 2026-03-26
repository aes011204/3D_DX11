#include "TransformModifier.h"
#include "UITransform.h"
#include "UI.h"

CTransformModifier::CTransformModifier(_float2 startPos, _float2 endPos, _float speed, _float Duration)
	:m_startPos(startPos), m_endPos(endPos), m_speed(speed), m_Duration(Duration)
{
}

CTransformModifier::~CTransformModifier()
{
}

void CTransformModifier::Tick(float fDeltaTime, CUI* pOwner)
{
	if (nullptr == m_pOwnerTransform)
	{
		m_pOwnerTransform = dynamic_cast<CUITransform*>(pOwner->Get_Component(g_strUITransformTag).get());
		//m_pOwnerTransform->SetAnchoredPos(m_startPos);
	}
	m_AccTime += fDeltaTime;

	_float fRatio = m_AccTime / m_Duration;


	_float2 result = {};
	result.x = lerp(m_startPos.x, m_endPos.x, fRatio);
	result.y = lerp(m_startPos.y, m_endPos.y, fRatio);

	m_pOwnerTransform->SetAnchoredPos(result);


	if (fRatio >= 1) 
	{
		fRatio = 1.f;
		m_bFinish = true;
	}

}

void CTransformModifier::OnAppear(float fDeltaTime, CUI* pOwner)
{
}

void CTransformModifier::OnDisappear(float fDeltaTime, CUI* pOwner)
{
}
