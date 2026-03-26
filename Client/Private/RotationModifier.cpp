#include "RotationModifier.h"
#include "UITransform.h"
#include "UI.h"
#include "Log_Manager.h"

CRotationModifier::CRotationModifier(_float speed)
	: 
	m_Speed(speed)
{

}
CRotationModifier::~CRotationModifier()
{
}

void CRotationModifier::Tick(float fDeltaTime, CUI* pOwner)
{
	if (nullptr == m_pOwnerTransform)
	{
		m_pOwnerTransform = dynamic_cast<CUITransform*>(pOwner->Get_Component(g_strUITransformTag).get());
	}

	//m_fAccTime += fDeltaTime;

	Vector2 size = {};

	_float result = -(m_Speed* fDeltaTime) + m_pOwnerTransform->Get_Rotation();
	
	m_pOwnerTransform->SetRotation(result);
	//m_pOwnerTransform->UpdateLayoutIfDirty();
	//CLog_Manager::GetInstance()->Add_Log_F(LOG_LEVEL::INFO,"dt: %.4f  acc: %.3f  scale: %.4f", fDeltaTime, m_fAccTime, sin);
}

void CRotationModifier::OnAppear(float fDeltaTime, CUI* pOwner)
{
}

void CRotationModifier::OnDisappear(float fDeltaTime, CUI* pOwner)
{

}
