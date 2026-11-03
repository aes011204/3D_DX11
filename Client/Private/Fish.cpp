#include "Fish.h"
#include "GameInstance.h"
#include "Model.h"
#include "Bounding_AABB.h"
#include "DInput_Manager.h"
#include "EventBus.h"
#include "MiniGame_Logic.h"
#include "WaterEff.h"

CFish::CFish(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject{ device ,context }
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

HRESULT CFish::Initialize(void* arg)
{

	FishDesc* fishDesc = static_cast<FishDesc*>(arg);

	m_fishDefID = fishDesc->fishDefID;
	m_initFishCount = fishDesc->fishCount;
	m_miniGameType = fishDesc->miniGameType;

	m_fishes.resize(fishDesc->fishCount);
	for(int i=0; i < fishDesc->fishCount ; i++)
	{
		m_fishes[i].height = m_gameInstance.lock()->Random(fishDesc->height.x, fishDesc->height.y);
		m_fishes[i].size = m_gameInstance.lock()->Random(fishDesc->size.x, fishDesc->size.y);
		m_fishes[i].speed = m_gameInstance.lock()->Random(fishDesc->speed.x, fishDesc->speed.y);
		m_fishes[i].radius = m_gameInstance.lock()->Random(fishDesc->radius.x, fishDesc->radius.y);
		m_fishes[i].alpha = m_gameInstance.lock()->Random(fishDesc->alphaTime.x, fishDesc->alphaTime.y);
		m_fishes[i].initPosRad = m_gameInstance.lock()->Random(XMConvertToRadians(0.f), XMConvertToRadians(179.f));

	}

	/*Set_InvenCtrl(fishDesc->InvenCtrl);*/



	if (FAILED(__super::Initialize(fishDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	CWaterEff::EffDesc eff = {};
	eff.type = EEffType::FISH;
	eff.target = static_pointer_cast<CFish>(shared_from_this());

	auto effObj = ((m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_WaterEff"), ETOI(ELevel::GAMEPLAY), L"Layer_Effect", &eff)));
	if (effObj == nullptr)
		return E_FAIL;




	return S_OK;
}

void CFish::Priority_Update(_float timeDelta)
{
}

void CFish::Update(_float timeDelta)
{
	 //TODO:: 물고기 갯수 줄어들어야 함


	if (m_isDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_isDirtyCom = false;
	}



	_matrix pivot  = XMLoadFloat4x4(m_transformCom->Get_WorldMatrix());

	m_acc += timeDelta;

	for(int i =0; i < m_initFishCount; i++)
	{

		//크자이공부
		//m_Fishs[i].FishMatrices;

		_matrix mat = XMMatrixIdentity();

		_matrix scale = XMMatrixScaling(1.f * m_fishes[i].size, 1.f * m_fishes[i].size, 1.f * m_fishes[i].size);


		_matrix rotY = XMMatrixRotationY((m_fishes[i].speed * m_acc + m_fishes[i].initPosRad)+XMConvertToRadians(90.f));



		float X = m_fishes[i].radius * sin(m_fishes[i].speed * m_acc + m_fishes[i].initPosRad);
		float Z = m_fishes[i].radius * cos(m_fishes[i].speed * m_acc + m_fishes[i].initPosRad);

		_matrix trans = XMMatrixTranslation(X, m_fishes[i].height, Z);

		XMStoreFloat4x4(&m_fishes[i].fishMatrices, scale * rotY * trans * pivot);
	}




	if(m_miniGameLogic !=nullptr)
	{
		m_miniGameLogic->Update(timeDelta);

		if (GetCurFishCount() == 0)
			Mark_Destroy();
	}




	int a = 1;

	m_colliderCom->Update(XMLoadFloat4x4(m_transformCom->Get_WorldMatrixPtr()));
}

void CFish::Late_Update(_float timeDelta)
{


	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CFish::Render()
{




	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;



	for (int i = 0; i < m_initFishCount; i++)
	{

		//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		//return E_FAIL;
		if (FAILED(m_shaderCom->Bind_Matrix("g_WorldMatrix", &m_fishes[i].fishMatrices)))
			return E_FAIL;

	size_t numMesh = m_modelCom->Get_NumMeshes();
		for (size_t i = 0; i < numMesh; i++)
		{
			m_modelCom->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
			////m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
			m_modelCom->Bind_Material(m_shaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);

			if (FAILED(m_shaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_modelCom->Render(i)))
				return E_FAIL;
		}

	}



#ifdef _DEBUG
	if (m_gameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_gameInstance.lock()->Add_DebugenderGroup(m_colliderCom);
	

#endif




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

void CFish::Change_Cam(shared_ptr<CGameObject>m_player)
{
	//Evt_ChangeCam event = {};
	//auto pLerp = make_shared<CAM_LERP_DESC>();
	//pLerp->eMode = CAM_MODE::LERP;
	//
	//_vector forward = m_Player->Get_TransformCom()->Get_State(STATE::LOOK);
	//XMStoreFloat3(&pLerp->vTargetPos, m_Player->Get_TransformCom()->Get_Position()
	//	- XMVector3Normalize(forward) * 3.f   // 살짝 뒤로
	//	+ XMVectorSet(0.f, 20.f, 0.f, 0.f));   // 위);
	////pLerp->vTargetRot = _float3(1.f, 1.f, 1.f);
	//pLerp->m_Target = m_Player;
	//pLerp->fDuration = 1.5f;
	//event.commands.push_back(pLerp);

	//auto pStop = make_shared<CAM_DESC>();
	//pStop->eMode = CAM_MODE::STOP;
	//event.commands.push_back(pStop);

	//m_pGameInstance.lock()->Get_EventBus()->Publish(event);
}

void CFish::RebindCom()
{
	
	//m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_modelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CFish::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
		return E_FAIL;

	//const LIGHT_DESC* pLightDesc = m_pGameInstance.lock()->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	////

	float farZ = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far", &farZ, sizeof(_float))))


	return S_OK;
}



HRESULT CFish::Ready_Components()
{

		// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
		if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
			return E_FAIL;



		// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
		if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Fish"), TEXT("Com_Model"), &m_modelCom, nullptr)))
			return E_FAIL;


		CBounding_AABB::BoundingAABBDesc		AABBDesc{};
		AABBDesc.extents = _float3(0.4f, 0.4f, 0.4f);
		AABBDesc.center = _float3(0.f, 1.f, 0.f);
		AABBDesc.myLayer = ECollisionLayer::FISH;
		AABBDesc.otherMask = ECollisionLayer::PLAYER;
		if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &m_colliderCom, &AABBDesc)))
			return E_FAIL;
		m_gameInstance.lock()->Add_Collider(m_colliderCom);
	return S_OK;
}

shared_ptr<CFish> CFish::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CFish> instance(new CFish(device, context), [](CFish* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFish");
	}
	return instance;
}


shared_ptr<CGameObject> CFish::Clone(void* arg)
{
	shared_ptr<CFish> instance(new CFish(*this), [](CFish* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CFish");
	}
	return instance;
}

void CFish::Free()
{
	m_fishes.clear();
	__super::Free();
}
