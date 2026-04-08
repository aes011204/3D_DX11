#include "Monster_Tantacle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"


CMonster_Tantacle::CMonster_Tantacle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CMonster_Tantacle::CMonster_Tantacle(const CMonster_Tantacle& prototype)
	: CGameObject{prototype}
{
}

HRESULT CMonster_Tantacle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Tantacle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_AnimIndex = 1;
	m_pModelCom->Set_Animation(m_AnimIndex, true);
	m_State = STATE::IDLE;



	//m_pSocketMatrix_AttackPoint = m_pModelCom->Get_BoneMatrixPtr(/*"l_botclaw_jnt"*/);

	return S_OK;
}

void CMonster_Tantacle::Priority_Update(_float fTimeDelta)
{
}

void CMonster_Tantacle::Update(_float fTimeDelta)
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


	m_Attack_Col->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_AttackPoint)));
	
	
}

void CMonster_Tantacle::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMonster_Tantacle::Render()
{

#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_pColliderCom->Render();

	m_Attack_Col->Render();

#endif



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




	return S_OK;
}

void CMonster_Tantacle::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{



	if(m_State == ATTACK)
	{
		if (self == m_Attack_Col)
		{
			int i = 0;
		}
	}



}
void CMonster_Tantacle::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{


	int i = 0;

	
}

void CMonster_Tantacle::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	int i = 0;

}

void CMonster_Tantacle::OnGui()
{
}

void CMonster_Tantacle::RebindCom()
{

	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CMonster_Tantacle::Bind_ShaderResources()
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

HRESULT CMonster_Tantacle::Ready_Components()
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
	SPhereDesc.vCenter = _float3(0.f, 0.f,0.f);
	SPhereDesc.MyLayer = COLLISION_LAYER::MONSTERATT;
	SPhereDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Hand_Collider_1"), &m_Attack_Col, &SPhereDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_Attack_Col);

	


	return S_OK;
}

shared_ptr<CMonster_Tantacle> CMonster_Tantacle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CMonster_Tantacle> pInstance(new CMonster_Tantacle(pDevice, pContext), [](CMonster_Tantacle* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Tantacle");
	}
	return pInstance;
}


shared_ptr<CGameObject> CMonster_Tantacle::Clone(void* pArg)
{
	shared_ptr<CMonster_Tantacle> pInstance(new CMonster_Tantacle(*this), [](CMonster_Tantacle* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Tantacle");
	}
	return pInstance;
}

void CMonster_Tantacle::Free()
{
	__super::Free();
}
