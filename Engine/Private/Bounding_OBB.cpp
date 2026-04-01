#include "Bounding_OBB.h"
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
	//const _char* pName = typeid(*pTarget).name();

	//if(false ==strcmp("class Engine::CBounding_OBB", pName))
	//{
	//	
	//}
	//else if(false == strcmp("class Engine::CBounding_OBB", pName))
	//{
	//	
	//}
	//else
	//{
	//	
	//}
	return _bool();
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
