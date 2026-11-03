#include "Bounding_Sphere.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "DebugDraw.h"
CBounding_Sphere::CBounding_Sphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBounding(pDevice, pContext)
{
}

HRESULT CBounding_Sphere::Initialize(BoundingDesc* pDesc)
{
	auto pBoundingDesc = static_cast<BoundingSphereDesc*>(pDesc);
	m_originalDesc = make_shared< BoundingSphere>(pBoundingDesc->center, pBoundingDesc->radius);
	m_desc = make_shared< BoundingSphere>(*m_originalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{

	m_originalDesc->Transform(*m_desc, WorldMatrix);

}

_bool CBounding_Sphere::Intersect(shared_ptr<CBounding>  pTarget)
{
	_bool isCollision = { false };
	const _char* pName = typeid(*pTarget).name();

	if(false ==strcmp("class Engine::CBounding_AABB", pName))
	{
		auto pAABB = dynamic_pointer_cast<CBounding_AABB>(pTarget);
		isCollision = m_desc->Intersects(*pAABB->Get_Desc());
	
	}
	else if(false == strcmp("class Engine::CBounding_OBB", pName))
	{
		auto pOBB = dynamic_pointer_cast<CBounding_OBB>(pTarget);
		isCollision = m_desc->Intersects(*pOBB->Get_Desc());


	}
	else
	{
		auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(pTarget);
		isCollision = m_desc->Intersects(*pSphere->Get_Desc());
	}
	return isCollision;
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_desc, vColor);
	return S_OK;

}
#endif

shared_ptr<CBounding_Sphere> CBounding_Sphere::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,
	BoundingDesc* pDesc)
{
	shared_ptr<CBounding_Sphere> pInstance(new CBounding_Sphere(pDevice, pContext), [](CBounding_Sphere* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
	}
	return pInstance;
}

void CBounding_Sphere::Free()
{
	CBounding::Free();
}
