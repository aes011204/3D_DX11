#include "Component.h"

#include "GameInstance.h"
#include "Entity.h"
#include "GameObject.h"
#include "UI.h"

CComponent::CComponent(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: m_device{ device }, m_context{ context },
	m_gameInstance{ CGameInstance::GetInstance() }
,  m_isCloned{ false }
{

}

CComponent::CComponent(const CComponent& rhs)
	: CBase(rhs),
	m_device{ rhs.m_device }, m_context{ rhs.m_context },
	m_gameInstance{ CGameInstance::GetInstance() }
	,m_isCloned(true)
/*,
	m_strProtoLevel{rhs.m_strProtoLevel},
	m_strProtoTag{rhs.m_strProtoTag}*/

{

}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* arg)
{
	return S_OK;
}

HRESULT CComponent::Set_Owner(shared_ptr<CEntity> owner)
{

	if (shared_ptr<CGameObject> gameObj = dynamic_pointer_cast<CGameObject>(owner))
	{
		m_GOwner = gameObj;
		m_GOwnerTransform = dynamic_cast<CTransform*>(gameObj->Get_Component(g_strTransformTag).get());

		return S_OK;
	}
	else if (shared_ptr<CUI> UIObj = dynamic_pointer_cast<CUI>(owner))
	{
		m_UOwner = UIObj;
		m_UOwnerUITransform = dynamic_cast<CUITransform*>(UIObj->Get_Component(g_strUITransformTag).get());

		return S_OK;
	}

	return E_FAIL;
}



void CComponent::Free()
{
	__super::Free();


}
