#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"
CBounding_AABB::CBounding_AABB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBounding(pDevice, pContext)
{
}

HRESULT CBounding_AABB::Initialize(BOUNDING_DESC* pDesc)
{
	auto pBoundingDesc = static_cast<BOUNDING_AABB_DESC*>(pDesc);
	m_pOriginalDesc = make_shared< BoundingBox>(pBoundingDesc->vCenter, pBoundingDesc->vExtents);
	m_pDesc = make_shared< BoundingBox>(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);

}

_bool CBounding_AABB::Intersect(shared_ptr<CBounding>  pTarget)
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
		auto pOBB = dynamic_pointer_cast<CBounding_OBB>(pTarget);
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
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pDesc, vColor);
	return S_OK;

}
#endif

shared_ptr<CBounding_AABB> CBounding_AABB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,
	BOUNDING_DESC* pDesc)
{
	shared_ptr<CBounding_AABB> pInstance(new CBounding_AABB(pDevice, pContext), [](CBounding_AABB* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
	}
	return pInstance;
}

void CBounding_AABB::Free()
{
	CBounding::Free();
}
