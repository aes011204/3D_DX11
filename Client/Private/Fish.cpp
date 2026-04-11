#include "Fish.h"
#include "GameInstance.h"
#include "Model.h"
#include "Bounding_AABB.h"
#include "DInput_Manager.h"
#include "EventBus.h"

CFish::CFish(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CFish::CFish(const CFish& prototype)
	: CGameObject{ prototype }
{
}

HRESULT CFish::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFish::Initialize(void* pArg)
{

	m_bIsSavableClass = true;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CFish::Priority_Update(_float fTimeDelta)
{
}

void CFish::Update(_float fTimeDelta)
{
	if (m_bIsDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_bIsDirtyCom = false;
	}
	int a = 1;

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CFish::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CFish::Render()
{
#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_pColliderCom->Render();


#endif




	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMesh; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}



	return S_OK;
}

void CFish::OnGui()
{



}

void CFish::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{



	CGameObject::OnBeginOverlap(self, other);
}

void CFish::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{

	CGameObject::OnEndOverlap(self, other);
}

void CFish::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{

	//if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_F))
	//{

	//	Evt_ChangeCam event = {};
	//	auto pLerp = make_shared<CAM_DESC>();
	//	pLerp->eMode = CAM_MODE::STOP;
	//	event.commands.push_back(pLerp);

	//	auto pStop = make_shared<CAM_DESC>();
	//	pStop->eMode = CAM_MODE::STOP;
	//	event.commands.push_back(pStop);

	//	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
	//	//m_NextState = E_PLAYERSTATE::FISHING;





	//}

	CGameObject::OnStayOverlap(self, other);
}

void CFish::Change_Cam(shared_ptr<CGameObject>m_Player)
{
	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;
	
	_vector forward = m_Player->Get_TransformCom()->Get_State(STATE::LOOK);
	XMStoreFloat3(&pLerp->vTargetPos, m_Player->Get_TransformCom()->Get_Position()
		- XMVector3Normalize(forward) * 3.f   // 살짝 뒤로
		+ XMVectorSet(0.f, 20.f, 0.f, 0.f));   // 위);
	//pLerp->vTargetRot = _float3(1.f, 1.f, 1.f);
	pLerp->m_Target = m_Player;
	pLerp->fDuration = 1.5f;
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);

	m_pGameInstance.lock()->Get_EventBus()->Publish(event);
}

void CFish::RebindCom()
{
	
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CFish::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	////
	
	return S_OK;
}



HRESULT CFish::Ready_Components()
{
	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fish"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;
	//if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
	//	return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		AABBDesc{};
	AABBDesc.vExtents = _float3(0.4f, 0.4f, 0.4f);
	AABBDesc.vCenter = _float3(0.f, 1.f, 0.f);
	AABBDesc.MyLayer = COLLISION_LAYER::FISH;
	AABBDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &m_pColliderCom, &AABBDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_pColliderCom);


	return S_OK;
}

shared_ptr<CFish> CFish::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CFish> pInstance(new CFish(pDevice, pContext), [](CFish* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFish");
	}
	return pInstance;
}


shared_ptr<CGameObject> CFish::Clone(void* pArg)
{
	shared_ptr<CFish> pInstance(new CFish(*this), [](CFish* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFish");
	}
	return pInstance;
}

void CFish::Free()
{
	__super::Free();
}
