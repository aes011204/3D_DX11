#include "PartObject.h"

CPartObject::CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CPartObject::CPartObject(const CPartObject& prototype)
	: CGameObject(prototype)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	auto pDesc = static_cast <PARTOBJ_DESC*>(pArg);

	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());


	return CGameObject::Initialize(pArg);
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
	
}

void CPartObject::Update(_float fTimeDelta)
{

}

void CPartObject::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CPartObject::Render()
{
	return S_OK;
}


void CPartObject::Free()
{
	__super::Free();
}
