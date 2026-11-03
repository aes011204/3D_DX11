#include "Collider.h"

#include "GameInstance.h"

CCollider::CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
	:CComponent(Prototype)
	, m_type{ Prototype.m_type }
#ifdef _DEBUG
	, m_batch(Prototype.m_batch)
	, m_effect(Prototype.m_effect)
	, m_inputLayout(Prototype.m_inputLayout)
#endif
{
}

HRESULT CCollider::Initialize_Prototype(EColliderType eType)
{
	m_type = eType;
#ifdef _DEBUG

	m_batch = new PrimitiveBatch<VertexPositionColor>(m_context.Get());
	m_effect = new BasicEffect(m_device.Get());
	m_effect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t iShaderByteCodeLength = {};

	m_effect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if(FAILED(m_device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_inputLayout)))
	{
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{

	auto pDesc = static_cast<CBounding::BoundingDesc*>(pArg);

	switch(m_type)
	{
	case EColliderType::AABB:
		m_bounding = CBounding_AABB::Create(m_device, m_context, pDesc);
		break;

	case EColliderType::OBB:
		m_bounding = CBounding_OBB::Create(m_device, m_context, pDesc);
		break;
	
	case EColliderType::SPHERE:
		m_bounding = CBounding_Sphere::Create(m_device, m_context, pDesc);
		break;
	}

	m_myLayer = pDesc->myLayer;
	m_otherMask = pDesc->otherMask;
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


		if (m_bounding != nullptr)
		{
			switch (m_type)
			{
			case EColliderType::AABB:
				if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_bounding))
				{
					auto pDesc = pAABB->Get_OriginalDesc();
					ImGui::Text("[AABB]");
					ImGui::DragFloat3("Center", (float*)&pDesc->Center, 0.01f);
					ImGui::DragFloat3("Extents", (float*)&pDesc->Extents, 0.01f);
				}
				break;

			case EColliderType::OBB:
				if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_bounding))
				{
					auto pDesc = pOBB->Get_OriginalDesc();
					ImGui::Text("[OBB]");
					ImGui::DragFloat3("Center", (float*)&pDesc->Center, 0.01f);
					ImGui::DragFloat3("Extents", (float*)&pDesc->Extents, 0.01f);
					//ImGui::DragFloat3("Degree", (float*)&pDesc->Orientation, 0.1f);
				}
				break;

			case EColliderType::SPHERE:
				if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_bounding))
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
	if (m_bounding != nullptr)
	{
		switch (m_type)
		{
		case EColliderType::AABB:
			if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_bounding)) {
				auto pDesc = pAABB->Get_OriginalDesc();
				j["Center"] = { pDesc->Center.x, pDesc->Center.y, pDesc->Center.z };
				j["Extents"] = { pDesc->Extents.x, pDesc->Extents.y, pDesc->Extents.z };
			}
			break;
	
		case EColliderType::OBB:
			if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_bounding)) {
				auto pDesc = pOBB->Get_OriginalDesc();
				j["Center"] = { pDesc->Center.x, pDesc->Center.y, pDesc->Center.z };
				j["Extents"] = { pDesc->Extents.x, pDesc->Extents.y, pDesc->Extents.z };
				//j["Orientation"] = { pDesc->Orientation.x, pDesc->Orientation.y, pDesc->Orientation.z, pDesc->Orientation.w };
			}
			break;
	
		case EColliderType::SPHERE:
			if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_bounding)) {
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
	if (j.contains("MyLayer")) m_myLayer = j["MyLayer"];
	if (j.contains("OtherMask")) m_otherMask = j["OtherMask"];
	
	//  Initialize에서 생성되어 있어야 함
	if (m_bounding != nullptr)
	{
		switch (m_type)
		{
		case EColliderType::AABB:
			if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_bounding)) {
				auto pDesc = pAABB->Get_OriginalDesc();
				pDesc->Center = { j["Center"][0], j["Center"][1], j["Center"][2] };
				pDesc->Extents = { j["Extents"][0], j["Extents"][1], j["Extents"][2] };
			}
			break;
	
		case EColliderType::OBB:
			if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_bounding)) {
				auto pDesc = pOBB->Get_OriginalDesc();
				pDesc->Center = { j["Center"][0], j["Center"][1], j["Center"][2] };
				pDesc->Extents = { j["Extents"][0], j["Extents"][1], j["Extents"][2] };
				//pDesc->Orientation = { j["Orientation"][0], j["Orientation"][1], j["Orientation"][2], j["Orientation"][3] };
			}
			break;
	
		case EColliderType::SPHERE:
			if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_bounding)) {
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
	m_bounding->Update(WorldMatrix);
}

_bool CCollider::Intersect(shared_ptr<CCollider> pTarget)
{
	
	return m_bounding->Intersect(pTarget->m_bounding);

}
bool CCollider::Is_Overlap(shared_ptr<CCollider> other) 
{
	return m_overlapColl.contains(other);
}

_float3 CCollider::Get_WorldCenter()
{
	switch (m_type)
	{
	case EColliderType::AABB:
		if (auto pAABB = dynamic_pointer_cast<CBounding_AABB>(m_bounding))
		{
			return pAABB->Get_Desc()->Center;
		}
		break;

	case EColliderType::OBB:
		if (auto pOBB = dynamic_pointer_cast<CBounding_OBB>(m_bounding))
		{
			return pOBB->Get_Desc()->Center;
		}
		break;

	case EColliderType::SPHERE:
		if (auto pSphere = dynamic_pointer_cast<CBounding_Sphere>(m_bounding))
		{
			return pSphere->Get_Desc()->Center;
		}
		break;
	}
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	auto GInst = m_gameInstance.lock();

	if (GInst->Get_IsDebug() == false)
		return S_OK;

	m_effect->SetWorld(XMMatrixIdentity());
	m_effect->SetView(XMLoadFloat4x4(GInst->Get_Transform(ED3DTS::VIEW)));
	m_effect->SetProjection(XMLoadFloat4x4(GInst->Get_Transform(ED3DTS::PROJ)));

	m_context->IASetInputLayout(m_inputLayout.Get());

	m_effect->Apply(m_context.Get());

	m_batch->Begin();

	m_bounding->Render(m_batch, false == m_isColl ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	m_batch->End();

	return S_OK;

}
#endif
shared_ptr<CCollider> CCollider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, EColliderType eType)
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
		delete m_batch;
		delete m_effect;
	}

	m_inputLayout.Reset();
#endif

}
