#include "TransformModifier.h"
#include "UITransform.h"
#include "UI.h"

CTransformModifier::CTransformModifier(_float2 startPos, _float2 endPos, _float speed, _float Duration)
	:m_startPos(startPos), m_endPos(endPos), m_speed(speed), m_duration(Duration)
{
}

CTransformModifier::~CTransformModifier()
{
}

void CTransformModifier::Tick(float deltaTime, CUI* owner)
{
	if (nullptr == m_ownerTransform)
	{
		m_ownerTransform = dynamic_cast<CUITransform*>(owner->Get_Component(g_strUITransformTag).get());
		//m_pOwnerTransform->SetAnchoredPos(m_startPos);
	}
	m_accTime += deltaTime;

	_float ratio = m_accTime / m_duration;


	_float2 result = {};
	result.x = lerp(m_startPos.x, m_endPos.x, ratio);
	result.y = lerp(m_startPos.y, m_endPos.y, ratio);

	m_ownerTransform->SetAnchoredPos(result);


	if (ratio >= 1) 
	{
		ratio = 1.f;
		m_isFinish = true;
	}

}

void CTransformModifier::OnAppear(float deltaTime, CUI* owner)
{
}

void CTransformModifier::OnDisappear(float deltaTime, CUI* owner)
{
}
