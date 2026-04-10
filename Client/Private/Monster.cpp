#include "Monster.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"


CMonster::CMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CMonster::CMonster(const CMonster& prototype)
	: CGameObject{prototype}
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_AnimIndex = 1;
	m_pModelCom->Set_Animation(m_AnimIndex, true);
	m_State = STATE::IDLE;


	m_pSocketMatrix_RightHand = m_pModelCom->Get_BoneMatrixPtr("r_botclaw_jnt");
	m_pSocketMatrix_LefttHand= m_pModelCom->Get_BoneMatrixPtr("l_botclaw_jnt");

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
}

void CMonster::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);


	if(m_pModelCom->Get_IsFinishAnim() == true)
	{
		int i = 0;
	}

	if(m_State == ATTACK && m_pModelCom->Get_IsFinishAnim()==true)
	{
		m_AnimIndex = 2;
		m_pModelCom->Set_Animation(m_AnimIndex, false);
		m_State = STATE::RELEASE;
	}
	else if(m_State == RELEASE && m_pModelCom->Get_IsFinishAnim() == true)
	{
		m_AnimIndex = 1;
		m_pModelCom->Set_Animation(m_AnimIndex, true);
		m_State = STATE::IDLE;
	}

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


	m_Hand_Collider_1->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_RightHand)));
	m_Hand_Collider_2->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_LefttHand)));
	
}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMonster::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for(size_t i=0; i< iNumMesh; i++)
	{
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
	m_pColliderCom->Render();

	m_Hand_Collider_1->Render();
	m_Hand_Collider_2->Render();

#endif




	return S_OK;
}

void CMonster::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if(m_State != STATE::ATTACK &&  self == m_pColliderCom)
	{
	m_AnimIndex = 0;

	m_pModelCom->Set_Animation(m_AnimIndex, false);
	m_State = ATTACK;
	
	}

	if(m_State == ATTACK)
	{
		if (self == m_Hand_Collider_2 || self == m_Hand_Collider_1)
		{
			dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner())->Get_Demage();
		}
	}


	int i = 0;

}
void CMonster::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{


	int i = 0;

	
}

void CMonster::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	int i = 0;

}

void CMonster::OnGui()
{
}

void CMonster::RebindCom()
{

	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CMonster::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CMonster::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FullBoatCrab"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;


	CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};
	OBBDesc.vExtents = _float3(2.f, 1.f, 2.f);
	OBBDesc.vRadians = _float3(0.f, 0.f, 0.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 5.f);
	OBBDesc.MyLayer = COLLISION_LAYER::TRIGGER;
	OBBDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_pColliderCom, &OBBDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_pColliderCom);

	CBounding_Sphere::BOUNDING_SPHERE_DESC		SPhereDesc{};
	
	SPhereDesc.fRadius = 2.f;
	SPhereDesc.vCenter = _float3(0.f, 2.f,0.f);
	SPhereDesc.MyLayer = COLLISION_LAYER::MONSTERATT;
	SPhereDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Hand_Collider_1"), &m_Hand_Collider_1 , &SPhereDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_Hand_Collider_1);


	CBounding_Sphere::BOUNDING_SPHERE_DESC		SPhereDesc1{};
	SPhereDesc1.fRadius = 2.f;
	SPhereDesc1.vCenter = _float3(0.f, -2.f, 0.f);
	SPhereDesc1.MyLayer = COLLISION_LAYER::MONSTERATT;
	SPhereDesc1.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Hand_Collider_2"), &m_Hand_Collider_2, &SPhereDesc1)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_Hand_Collider_2);



	return S_OK;
}

shared_ptr<CMonster> CMonster::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CMonster> pInstance(new CMonster(pDevice, pContext), [](CMonster* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
	}
	return pInstance;
}


shared_ptr<CGameObject> CMonster::Clone(void* pArg)
{
	shared_ptr<CMonster> pInstance(new CMonster(*this), [](CMonster* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
	}
	return pInstance;
}

void CMonster::Free()
{
	__super::Free();
}
