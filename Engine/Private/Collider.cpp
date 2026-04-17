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

void CCollider::OnGui()
{
	CComponent::OnGui();


	if (ImGui::TreeNode("Collider Data"))
	{
	
		//ImGui::InputInt("Other Mask", (int*)&m_OtherMask);
		//ImGui::InputInt("My Layer", (int*)&m_MyLayer);

		ImGui::Separator();


		if (m_pBounding != nullptr)
		{
			switch (m_eType)
			{
			case COLLIDER::AABB:
				if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_pBounding))
				{
					auto pDesc = pAABB->Get_OriginalDesc();
					ImGui::Text("[AABB]");
					ImGui::DragFloat3("Center", (float*)&pDesc->Center, 0.01f);
					ImGui::DragFloat3("Extents", (float*)&pDesc->Extents, 0.01f);
				}
				break;

			case COLLIDER::OBB:
				if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_pBounding))
				{
					auto pDesc = pOBB->Get_OriginalDesc();
					ImGui::Text("[OBB]");
					ImGui::DragFloat3("Center", (float*)&pDesc->Center, 0.01f);
					ImGui::DragFloat3("Extents", (float*)&pDesc->Extents, 0.01f);
					//ImGui::DragFloat3("Degree", (float*)&pDesc->Orientation, 0.1f);
				}
				break;

			case COLLIDER::SPHERE:
				if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_pBounding))
				{
					auto pDesc = pSphere->Get_OriginalDesc();
					ImGui::Text("[Sphere]");
					ImGui::DragFloat3("Center", (float*)&pDesc->Center, 0.01f);
					ImGui::DragFloat("Radius", &pDesc->Radius, 0.01f);
				}
				break;
			}
		}
		ImGui::TreePop();
	}
}


void CCollider::Save_ToJson(nlohmann::json& j)
{
	CComponent::Save_ToJson(j);
	if (m_pBounding != nullptr)
	{
		switch (m_eType)
		{
		case COLLIDER::AABB:
			if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_pBounding)) {
				auto pDesc = pAABB->Get_OriginalDesc();
				j["Center"] = { pDesc->Center.x, pDesc->Center.y, pDesc->Center.z };
				j["Extents"] = { pDesc->Extents.x, pDesc->Extents.y, pDesc->Extents.z };
			}
			break;
	
		case COLLIDER::OBB:
			if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_pBounding)) {
				auto pDesc = pOBB->Get_OriginalDesc();
				j["Center"] = { pDesc->Center.x, pDesc->Center.y, pDesc->Center.z };
				j["Extents"] = { pDesc->Extents.x, pDesc->Extents.y, pDesc->Extents.z };
				//j["Orientation"] = { pDesc->Orientation.x, pDesc->Orientation.y, pDesc->Orientation.z, pDesc->Orientation.w };
			}
			break;
	
		case COLLIDER::SPHERE:
			if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_pBounding)) {
				auto pDesc = pSphere->Get_OriginalDesc();
				j["Center"] = { pDesc->Center.x, pDesc->Center.y, pDesc->Center.z };
				j["Radius"] = pDesc->Radius;
			}
			break;
		}
	}
}

void CCollider::Load_FromJson(nlohmann::json& j)
{
	CComponent::Load_FromJson(j);
	if (j.contains("MyLayer")) m_MyLayer = j["MyLayer"];
	if (j.contains("OtherMask")) m_OtherMask = j["OtherMask"];
	
	//  Initialize에서 생성되어 있어야 함
	if (m_pBounding != nullptr)
	{
		switch (m_eType)
		{
		case COLLIDER::AABB:
			if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_pBounding)) {
				auto pDesc = pAABB->Get_OriginalDesc();
				pDesc->Center = { j["Center"][0], j["Center"][1], j["Center"][2] };
				pDesc->Extents = { j["Extents"][0], j["Extents"][1], j["Extents"][2] };
			}
			break;
	
		case COLLIDER::OBB:
			if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_pBounding)) {
				auto pDesc = pOBB->Get_OriginalDesc();
				pDesc->Center = { j["Center"][0], j["Center"][1], j["Center"][2] };
				pDesc->Extents = { j["Extents"][0], j["Extents"][1], j["Extents"][2] };
				//pDesc->Orientation = { j["Orientation"][0], j["Orientation"][1], j["Orientation"][2], j["Orientation"][3] };
			}
			break;
	
		case COLLIDER::SPHERE:
			if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_pBounding)) {
				auto pDesc = pSphere->Get_OriginalDesc();
				pDesc->Center = { j["Center"][0], j["Center"][1], j["Center"][2] };
				pDesc->Radius = j["Radius"];
			}
			break;
		}
	}
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

_float3 CCollider::Get_WorldCenter()
{
	switch (m_eType)
	{
	case COLLIDER::AABB:
		if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_pBounding))
		{
			return pAABB->Get_Desc()->Center;
		}
		break;

	case COLLIDER::OBB:
		if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_pBounding))
		{
			return pOBB->Get_Desc()->Center;
		}
		break;

	case COLLIDER::SPHERE:
		if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_pBounding))
		{
			return pSphere->Get_Desc()->Center;
		}
		break;
	}
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
