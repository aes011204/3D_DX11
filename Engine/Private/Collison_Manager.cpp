#include "Collison_Manager.h"

CCollision_Manager::CCollision_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
}

CCollision_Manager::~CCollision_Manager()
{
	Free();
}

HRESULT CCollision_Manager::Initialize()
{
}

void CCollision_Manager::Update(_fmatrix WorldMatrix)
{
}

void CCollision_Manager::Add_Collider(shared_ptr<CCollider> coll)
{
	if (coll == nullptr)
		return
		m_Colliders.push_back(coll);
}

void CCollision_Manager::Remove_Collider(shared_ptr<CCollider> coll)
{
}

unique_ptr<CCollision_Manager> CCollision_Manager::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CCollision_Manager> pInstance(new CCollision_Manager(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
	}
	return pInstance;
}

void CCollision_Manager::Free()
{
	CBase::Free();
}
