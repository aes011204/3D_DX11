#include "Mon_R_Act.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"


CMon_R_Act::CMon_R_Act(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CMon_R_Act::CMon_R_Act(const CMon_R_Act& prototype)
	: CGameObject{prototype}
{
}

HRESULT CMon_R_Act::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMon_R_Act::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_turnSpeed = 2.f;
	m_isColl = false;


	m_player = m_gameInstance.lock()->Get_GameObject(
		ETOI(ELevel::GAMEPLAY),
		L"Layer_Player",0
	);

	_float3 targetPos = {};
	XMStoreFloat3(&targetPos,m_player.lock()->Get_TransformCom()->Get_Position());

	float x = m_gameInstance.lock()->Random(-1, 1);
	float z = m_gameInstance.lock()->Random(-1, 1);
	_vector finalPos =XMVectorSet(targetPos.x + x, 0.f, targetPos.z + z, 1.f);

	m_transformCom->Set_Position(finalPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_animIndex = 0;
	m_modelCom->Set_Animation(m_animIndex, false);
	//m_State = STATE::IDLE;


	m_socketMatrix = m_modelCom->Get_BoneMatrixPtr("chin_jnt");


	m_curRot = m_transformCom->Get_Quaternion();

	m_gameInstance.lock()->Play_Loop(L"Leviathan_FinaleAttack");
	



	return S_OK;
}

void CMon_R_Act::Priority_Update(_float fTimeDelta)
{
}

void CMon_R_Act::Update(_float fTimeDelta)
{
	m_modelCom->Play_Animation(fTimeDelta);

	if(m_player.lock()&& m_isColl==false)
	{
	_vector TargetPos = m_player.lock()->Get_TransformCom()->Get_Position();
	m_transformCom->Set_Position(TargetPos );

	//m_pTransformCom->Set_Quaternion(m_pPlayer.lock()->Get_TransformCom()->Get_Quaternion()+m_pCurRot);
	}



	

	m_collider->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_socketMatrix)));

	
}

void CMon_R_Act::Late_Update(_float fTimeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMon_R_Act::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_modelCom->Get_NumMeshes();

	for(size_t i=0; i< iNumMesh; i++)
	{
		m_modelCom->Bind_Material(m_shaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);

		m_modelCom->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		m_modelCom->Bind_BoneMatrices(m_shaderCom, "g_BoneMatrices", i);

	if (FAILED(m_shaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_modelCom->Render(i)))
		return E_FAIL;
	}



#ifdef _DEBUG
	if (m_gameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_gameInstance.lock()->Add_DebugenderGroup(m_collider);
#endif


	return S_OK;
}

void CMon_R_Act::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
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
			dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner())->Get_Damage(4);

			m_isColl = true;
			m_gameInstance.lock()->Stop(L"Leviathan_Rumble_Loop",1.5);
			m_gameInstance.lock()->Stop(L"Leviathan_Distant_Call",1.5f);
			m_gameInstance.lock()->Stop(L"Leviathan_FinaleAttack",1.5f);

		}
	}



}
void CMon_R_Act::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{


	int i = 0;

	
}

void CMon_R_Act::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	int i = 0;

}

void CMon_R_Act::OnGui()
{
	float dist = XMVectorGetX(
		XMVector3Length(
			m_player.lock()->Get_TransformCom()->Get_Position()
			- m_transformCom->Get_Position()
		)
	);

	ImGui::Text("Dist: %.2f", dist);
}
void CMon_R_Act::RebindCom()
{

	m_textureCom = Get_Component<CTexture>(L"Com_Texture");
	m_modelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CMon_R_Act::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CMon_R_Act::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act"), TEXT("Com_Model"), &m_modelCom, nullptr)))
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
	
	SPhereDesc.radius = 2.f;
	SPhereDesc.center = _float3(0.f, 0.f,0.f);
	SPhereDesc.myLayer = ECollisionLayer::MONSTERATT;
	SPhereDesc.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), &m_collider , &SPhereDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_collider);
	
	return S_OK;
}

shared_ptr<CMon_R_Act> CMon_R_Act::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CMon_R_Act> pInstance(new CMon_R_Act(pDevice, pContext), [](CMon_R_Act* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMon_R_Act");
	}
	return pInstance;
}


shared_ptr<CGameObject> CMon_R_Act::Clone(void* pArg)
{
	shared_ptr<CMon_R_Act> pInstance(new CMon_R_Act(*this), [](CMon_R_Act* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMon_R_Act");
	}
	return pInstance;
}

void CMon_R_Act::Free()
{
	__super::Free();
}
