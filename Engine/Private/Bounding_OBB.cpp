#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"
CBounding_OBB::CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBounding(pDevice, pContext)
{
}

HRESULT CBounding_OBB::Initialize(BOUNDING_DESC* pDesc)
{
	auto pBoundingDesc = static_cast<BOUNDING_OBB_DESC*>(pDesc);

	_float4 vRotation = {};

	XMStoreFloat4(&vRotation,
		XMQuaternionRotationRollPitchYaw(pBoundingDesc->vRadians.x, pBoundingDesc->vRadians.y, pBoundingDesc->vRadians.z));

	m_pOriginalDesc = make_shared< BoundingOrientedBox>(pBoundingDesc->vCenter, pBoundingDesc->vExtents, vRotation);
	m_pDesc = make_shared< BoundingOrientedBox>(*m_pOriginalDesc);



	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	
	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);

}

_bool CBounding_OBB::Intersect(shared_ptr<CBounding>  pTarget)
{
	_bool isCollision = { false };
	const _char* pName = typeid(*pTarget).name();

	if(false ==strcmp("class Engine::CBounding_AABB", pName))
	{
		auto pAABB = dynamic_pointer_cast<CBounding_AABB>(pTarget);
		isCollision = m_pDesc->Intersects(*pAABB->Get_Desc());
	}
	else if(false == strcmp("class Engine::CBounding_OBB", pName))
	{
		auto pOBB = (dynamic_pointer_cast<CBounding_OBB>(pTarget));
		isCollision = m_pDesc->Intersects(*pOBB->Get_Desc());
	}
	else
	{
		auto pSPHERE = dynamic_pointer_cast<CBounding_Sphere>(pTarget);
		isCollision = m_pDesc->Intersects(*pSPHERE->Get_Desc());

	}
	return isCollision;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pDesc, vColor);
	return S_OK;

}
#endif

shared_ptr<CBounding_OBB> CBounding_OBB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,
	BOUNDING_DESC* pDesc)
{
	shared_ptr<CBounding_OBB> pInstance(new CBounding_OBB(pDevice, pContext), [](CBounding_OBB* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
	}
	return pInstance;
}

void CBounding_OBB::Free()
{
	CBounding::Free();
}
