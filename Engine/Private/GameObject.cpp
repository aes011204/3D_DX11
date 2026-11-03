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

	



	m_transformCom = CTransform::Create(m_device, m_context);
	if (nullptr == m_transformCom)
		return E_FAIL;

	if (FAILED(m_transformCom->Initialize(pArg)))
		return E_FAIL;

	m_transformCom->SetDefaultNameFromThisType();

	if (nullptr == Get_Component(g_strTransformTag))
	{
		m_components.emplace(g_strTransformTag, m_transformCom);
		
	}

	if (nullptr != pArg)
	{
		GameObjectDesc* pDesc = static_cast<GameObjectDesc*> (pArg);
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
