#include "Component.h"

#include "GameInstance.h"
#include "Entity.h"
#include "GameObject.h"
#include "UI.h"

CComponent::CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_device{ pDevice }, m_context{ pContext },
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

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CComponent::Set_Owner(shared_ptr<CEntity> pOwner)
{

	if (shared_ptr<CGameObject> GObj = dynamic_pointer_cast<CGameObject>(pOwner))
	{
		m_GOwner = GObj;
		m_GOwnerTransform = dynamic_cast<CTransform*>(GObj->Get_Component(g_strTransformTag).get());

		return S_OK;
	}
	else if (shared_ptr<CUI> UObj = dynamic_pointer_cast<CUI>(pOwner))
	{
		m_UOwner = UObj;
		m_UOwnerUITransform = dynamic_cast<CUITransform*>(UObj->Get_Component(g_strUITransformTag).get());

		return S_OK;
	}

	return E_FAIL;
}



void CComponent::Free()
{
	__super::Free();


}
