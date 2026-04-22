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

	m_TurnSpeed = 2.f;
	m_IsColl = false;


	m_pPlayer = m_pGameInstance.lock()->Get_GameObject(
		ETOI(LEVEL::GAMEPLAY),
		L"Layer_Player",0
	);

	_float3 targetPos = {};
	XMStoreFloat3(&targetPos,m_pPlayer.lock()->Get_TransformCom()->Get_Position());

	float x = m_pGameInstance.lock()->Random(-1, 1);
	float z = m_pGameInstance.lock()->Random(-1, 1);
	_vector finalPos =XMVectorSet(targetPos.x + x, 0.f, targetPos.z + z, 1.f);

	m_pTransformCom->Set_Position(finalPos);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_AnimIndex = 0;
	m_pModelCom->Set_Animation(m_AnimIndex, false);
	//m_State = STATE::IDLE;


	m_pSocketMatrix = m_pModelCom->Get_BoneMatrixPtr("chin_jnt");


	m_pCurRot = m_pTransformCom->Get_Quaternion();

	return S_OK;
}

void CMon_R_Act::Priority_Update(_float fTimeDelta)
{
}

void CMon_R_Act::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if(m_pPlayer.lock())
	{
	_vector TargetPos = m_pPlayer.lock()->Get_TransformCom()->Get_Position();
	m_pTransformCom->Set_Position(TargetPos );

	//m_pTransformCom->Set_Quaternion(m_pPlayer.lock()->Get_TransformCom()->Get_Quaternion()+m_pCurRot);
	}



	

	m_Collider->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix)));

	
}

void CMon_R_Act::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMon_R_Act::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for(size_t i=0; i< iNumMesh; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(i)))
		return E_FAIL;
	}



#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_pGameInstance.lock()->Add_DebugenderGroup(m_Collider);
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
		if (other->Get_MyLayer()==COLLISION_LAYER::PLAYER)
		{
			dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner())->Get_Demage();
			m_IsColl = true;
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
			m_pPlayer.lock()->Get_TransformCom()->Get_Position()
			- m_pTransformCom->Get_Position()
		)
	);

	ImGui::Text("Dist: %.2f", dist);
}
void CMon_R_Act::RebindCom()
{

	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CMon_R_Act::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
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

	float fFar = m_pGameInstance.lock()->Get_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Far", &fFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMon_R_Act::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
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

	CBounding_Sphere::BOUNDING_SPHERE_DESC		SPhereDesc{};
	
	SPhereDesc.fRadius = 2.f;
	SPhereDesc.vCenter = _float3(0.f, 0.f,0.f);
	SPhereDesc.MyLayer = COLLISION_LAYER::MONSTERATT;
	SPhereDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), &m_Collider , &SPhereDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_Collider);
	
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
