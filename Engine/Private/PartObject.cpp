#include "PartObject.h"

CPartObject::CPartObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject{ device ,context }
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

HRESULT CPartObject::Initialize(void* arg)
{
	auto desc = static_cast <PartObjDesc*>(arg);

	m_parentMatrix = desc->parentMatrix;

	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_combinedWorldMatrix, XMMatrixIdentity());


	return CGameObject::Initialize(arg);
}

void CPartObject::Priority_Update(_float timeDelta)
{
	
}

void CPartObject::Update(_float timeDelta)
{

}

void CPartObject::Late_Update(_float timeDelta)
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
