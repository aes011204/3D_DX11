#include "GameObject.h"
#include "Transform.h"
#include "Shader.h"


CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEntity( pDevice ,pContext )
{
	
}

CGameObject::CGameObject(const CGameObject& prototype)
	:CEntity(prototype)/*,
	m_strProtoLevel{ prototype.m_strProtoLevel },
	m_strProtoTag{ prototype.m_strProtoTag }*/
{
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	//m_fX = pDesc->fX;
	//m_fY = pDesc->fY;
	//m_fSizeX = pDesc->fSizeX;
	//m_fSizeY = pDesc->fSizeY;

	



	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->SetDefaultNameFromThisType();

	if (nullptr == Get_Component(g_strTransformTag))
	{
		m_Components.emplace(g_strTransformTag, m_pTransformCom);
		
	}

	if (nullptr != pArg)
	{
		GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*> (pArg);
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
