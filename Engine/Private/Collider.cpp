#include "Collider.h"

#include "GameInstance.h"

CCollider::CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
	:CComponent(Prototype)
	, m_eType{ Prototype.m_eType }
#ifdef _DEBUG
	, m_pBatch(Prototype.m_pBatch)
	, m_pEffect(Prototype.m_pEffect)
	, m_pInputLayout(Prototype.m_pInputLayout)
#endif
{
}

HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
	m_eType = eType;
#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext.Get());
	m_pEffect = new BasicEffect(m_pDevice.Get());
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if(FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
	{
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{

	auto pDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

	switch(m_eType)
	{
	case COLLIDER::AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pDesc);
		break;

	case COLLIDER::OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pDesc);
		break;
	
	case COLLIDER::SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pDesc);
		break;
	}

	m_MyLayer = pDesc->MyLayer;
	m_OtherMask = pDesc->OtherMask;
	//if (m_MyLayer != 0)
	//	m_pGameInstance.lock()->Add_Collider(static_pointer_cast<CCollider>(shared_from_this()));

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	m_pBounding->Update(WorldMatrix);
}

_bool CCollider::Intersect(shared_ptr<CCollider> pTarget)
{
	
	return m_pBounding->Intersect(pTarget->m_pBounding);

}
bool CCollider::Is_Overlap(shared_ptr<CCollider> other) 
{
	return m_overlapColl.contains(other);
}


#ifdef _DEBUG
HRESULT CCollider::Render()
{
	auto GInst = m_pGameInstance.lock();

	if (GInst->Get_IsDebug() == false)
		return S_OK;

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(XMLoadFloat4x4(GInst->Get_Transform(D3DTS::VIEW)));
	m_pEffect->SetProjection(XMLoadFloat4x4(GInst->Get_Transform(D3DTS::PROJ)));

	m_pContext->IASetInputLayout(m_pInputLayout.Get());

	m_pEffect->Apply(m_pContext.Get());

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch, false == m_isColl ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	m_pBatch->End();

	return S_OK;

}
#endif
shared_ptr<CCollider> CCollider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, COLLIDER eType)
{
	shared_ptr<CCollider> pInstance(new CCollider(pDevice, pContext), [](CCollider* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
	}
	return pInstance;
}

shared_ptr<CComponent> CCollider::Clone(void* pArg)
{
	shared_ptr<CCollider> pInstance(new CCollider(*this), [](CCollider* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
	}
	return pInstance;
}

void CCollider::Free()
{
	CComponent::Free();


#ifdef _DEBUG

	if (false == m_isCloned)
	{
		delete m_pBatch;
		delete m_pEffect;
	}

	m_pInputLayout.Reset();
#endif

}
