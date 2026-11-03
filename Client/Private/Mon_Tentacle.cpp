#include "Mon_Tentacle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"


CMon_Tentacle::CMon_Tentacle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CMon_Tentacle::CMon_Tentacle(const CMon_Tentacle& prototype)
	: CGameObject{prototype}
{
}

HRESULT CMon_Tentacle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMon_Tentacle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_TurnSpeed = 2.f;
	m_IsColl = false;


	m_pPlayer = m_gameInstance.lock()->Get_GameObject(
		ETOI(LEVEL::GAMEPLAY),
		L"Layer_Player",0
	);

	_float3 targetPos = {};
	XMStoreFloat3(&targetPos,m_pPlayer.lock()->Get_TransformCom()->Get_Position());

	float x = m_gameInstance.lock()->Random(-1.5, 1.5);
	float z = m_gameInstance.lock()->Random(-1.5, 1.5);
	_vector finalPos =XMVectorSet(targetPos.x + x, 0.f, targetPos.z + z, 1.f);

	m_transformCom->Set_Position(finalPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_AnimIndex = 0;
	m_pModelCom->Set_Animation(m_AnimIndex, false);
	//m_State = STATE::IDLE;


	m_pSocketMatrix_Top = m_pModelCom->Get_BoneMatrixPtr("Bone.011");
	m_pSocketMatrix_Mid = m_pModelCom->Get_BoneMatrixPtr("Bone.009");
	m_pSocketMatrix_Btm = m_pModelCom->Get_BoneMatrixPtr("Bone.007");
	m_pSocketMatrix_Btm_F = m_pModelCom->Get_BoneMatrixPtr("Bone.005");

	return S_OK;
}

void CMon_Tentacle::Priority_Update(_float fTimeDelta)
{
}

void CMon_Tentacle::Update(_float fTimeDelta)
{
	if(m_Flag==false)
	{
	m_gameInstance.lock()->Play_Once(L"OozeTentacleAttack");
	m_Flag = true;
	}

	_vector dir;
	m_pModelCom->Play_Animation(fTimeDelta);

	_int curFrame = m_pModelCom->Get_CurrentFrame();
	
	_vector TargetPos = m_pPlayer.lock()->Get_TransformCom()->Get_Position();
	if (curFrame < 140)
	{

		dir = XMVector3Normalize(TargetPos - m_transformCom->Get_Position());
		dir = XMVectorSetY(dir, 0.f);
		dir = XMVector3Normalize(dir);
		m_LastDir = dir;
	}
	else
	{
		dir = m_LastDir;
	}
	_vector Look = m_transformCom->Get_State(Engine::ETransformState::LOOK);

	_vector NewLook  = XMVector3Normalize(XMVectorLerp(Look, dir , fTimeDelta * m_TurnSpeed));
	
	m_transformCom->LookAt(m_transformCom->Get_Position() +NewLook);



	if (m_pModelCom->Get_IsFinishAnim() == true )
	{
		if (20.f <= XMVectorGetX(XMVector3Length(TargetPos - m_transformCom->Get_Position())) || m_IsColl == true)
		{
			Mark_Destroy();
		}
		else
		{
			m_pModelCom->Set_Animation(m_AnimIndex, false);
			m_Flag = false;
		}
	}
	
	

	m_Btm_Collider->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_Btm)));
	m_Btm_F_Collider->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_Btm_F)));


	m_Top_Collider->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_Top)));
	m_Mid_Collider->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_Mid)));

}

void CMon_Tentacle::Late_Update(_float fTimeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CMon_Tentacle>(shared_from_this()));
}

HRESULT CMon_Tentacle::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for(size_t i=0; i< iNumMesh; i++)
	{
		m_pModelCom->Bind_Material(m_shaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);
		m_pModelCom->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		m_pModelCom->Bind_BoneMatrices(m_shaderCom, "g_BoneMatrices", i);

	if (FAILED(m_shaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(i)))
		return E_FAIL;
	}




#ifdef _DEBUG
	if (m_gameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_gameInstance.lock()->Add_DebugenderGroup(m_Btm_F_Collider);
	m_gameInstance.lock()->Add_DebugenderGroup(m_Btm_Collider);
	m_gameInstance.lock()->Add_DebugenderGroup(m_Top_Collider);
	m_gameInstance.lock()->Add_DebugenderGroup(m_Mid_Collider);

#endif




	return S_OK;
}

void CMon_Tentacle::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	//if(m_State != STATE::ATTACK &&  self == m_pColliderCom)
	//{
	//m_AnimIndex = 0;
	//
	//m_pModelCom->Set_Animation(m_AnimIndex, false);
	//m_State = ATTACK;
	//
	//}

	//if(m_State == ATTACK)
	{
		if (other->Get_MyLayer()==ECollisionLayer::PLAYER)
		{
			dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner())->Get_Damage();
			m_IsColl = true;
		}
	}



}
void CMon_Tentacle::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{


	int i = 0;

	
}

void CMon_Tentacle::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	int i = 0;

}

void CMon_Tentacle::OnGui()
{
	float dist = XMVectorGetX(
		XMVector3Length(
			m_pPlayer.lock()->Get_TransformCom()->Get_Position()
			- m_transformCom->Get_Position()
		)
	);

	ImGui::Text("Dist: %.2f", dist);
}
void CMon_Tentacle::RebindCom()
{

	m_textureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CMon_Tentacle::Bind_ShaderResources()
{
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
		return E_FAIL;

	/*const LIGHT_DESC* pLightDesc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;*/

	float fFar = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far", &fFar, sizeof(_float))))
		return E_FAIL;

	float emissive = 4.f;
	if (FAILED(m_shaderCom->Bind_RawValue("g_EmissiveStrength", &emissive, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMon_Tentacle::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Tentacle"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;


	/*CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};
	OBBDesc.vExtents = _float3(0.5f, 0.5f, .5f);
	OBBDesc.vRadians = _float3(0.f, 0.f, 0.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	OBBDesc.MyLayer = COLLISION_LAYER::TRIGGER;
	OBBDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_pColliderCom, &OBBDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_pColliderCom);*/

	CBounding_Sphere::BoundingSphereDesc		SPhereDesc{};
	
	SPhereDesc.radius = .5f;
	SPhereDesc.center = _float3(0.f, 0.f,0.f);
	SPhereDesc.myLayer = ECollisionLayer::MONSTERATT;
	SPhereDesc.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Top_Collider"), &m_Top_Collider , &SPhereDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_Top_Collider);


	CBounding_Sphere::BoundingSphereDesc		SPhereDesc1{};
	SPhereDesc1.radius = .75f;
	SPhereDesc1.center = _float3(0.f, 0.f, 0.f);
	SPhereDesc1.myLayer = ECollisionLayer::MONSTERATT;
	SPhereDesc1.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Mid_Collider"), &m_Mid_Collider, &SPhereDesc1)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_Mid_Collider);

	CBounding_Sphere::BoundingSphereDesc		SPhereDesc3{};

	SPhereDesc3.radius = 1.f;
	SPhereDesc3.center = _float3(0.f, 0.f, 0.f);
	SPhereDesc3.myLayer = ECollisionLayer::MONSTERATT;
	SPhereDesc3.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Btm_Collider"), &m_Btm_Collider, &SPhereDesc3)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_Btm_Collider);


	CBounding_Sphere::BoundingSphereDesc		SPhereDesc4{};

	SPhereDesc4.radius = 1.f;
	SPhereDesc4.center = _float3(0.f, 0.f, 0.f);
	SPhereDesc4.myLayer = ECollisionLayer::MONSTERATT;
	SPhereDesc4.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Btm_F_Collider"), &m_Btm_F_Collider, &SPhereDesc4)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_Btm_F_Collider);

	return S_OK;
}

shared_ptr<CMon_Tentacle> CMon_Tentacle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CMon_Tentacle> pInstance(new CMon_Tentacle(pDevice, pContext), [](CMon_Tentacle* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMon_Tentacle");
	}
	return pInstance;
}


shared_ptr<CGameObject> CMon_Tentacle::Clone(void* pArg)
{
	shared_ptr<CMon_Tentacle> pInstance(new CMon_Tentacle(*this), [](CMon_Tentacle* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMon_Tentacle");
	}
	return pInstance;
}

void CMon_Tentacle::Free()
{
	__super::Free();
}
