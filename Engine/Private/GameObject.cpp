#include "GameObject.h"
#include "Transform.h"


CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEntity( pDevice ,pContext )
{
	
}

CGameObject::CGameObject(const CGameObject& prototype)
	:CEntity(prototype)
{
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*> (pArg);


	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->SetDefaultNameFromThisType();

	if (nullptr == Get_Component(g_strTransformTag))
	{
		m_Components.emplace(g_strTransformTag, m_pTransformCom);
		//Safe_AddRef(m_pTransformCom);
	}

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	//Safe_Release(m_pTransformCom);
}
