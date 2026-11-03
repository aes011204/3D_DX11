#include "Monster.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"


CMonster::CMonster(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject{ device ,context }
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

HRESULT CMonster::Initialize(void* arg)
{
	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_animIndex = 1;
	m_modelCom->Set_Animation(m_animIndex, true);
	m_state = EState::IDLE;


	m_socketMatrixRightHand = m_modelCom->Get_BoneMatrixPtr("r_botclaw_jnt");
	m_socketMatrixLeftHand= m_modelCom->Get_BoneMatrixPtr("l_botclaw_jnt");


	_float3 pos = _float3(142.3f, -0.1f, -3.8f);
	m_transformCom->Set_Position(XMLoadFloat3(&pos));
	m_transformCom->Set_RotationDegree(_float3(0.f,-68.f,0.f));
	m_transformCom->Update_WorldMatrix();

	m_handCollider1->Set_CollisionActive(false);
	m_handCollider2->Set_CollisionActive(false);

	return S_OK;
}

void CMonster::Priority_Update(_float timeDelta)
{
}

void CMonster::Update(_float timeDelta)
{
	m_modelCom->Play_Animation(timeDelta);


	if(m_modelCom->Get_IsFinishAnim() == true)
	{
		int i = 0;

	}


	if (m_state == ATTACK)
	{
		_int curFrame = m_modelCom->Get_CurrentFrame();

		if (m_attackPrevFrame < 80 && curFrame >= 80)
		{
			m_gameInstance.lock()->Play_Once(L"WreckMonsterAttack1");
			m_handCollider1->Set_CollisionActive(true);
			m_handCollider2->Set_CollisionActive(true);
		}

		if (m_attackPrevFrame < 126 && curFrame >= 126)
			m_gameInstance.lock()->Play_Once(L"WreckMonsterSwipeAttack");

		if (m_attackPrevFrame < 170 && curFrame >= 170)
			m_gameInstance.lock()->Play_Once(L"WreckMonsterSwipeAttack");

		m_attackPrevFrame = curFrame;
		
		if (m_modelCom->Get_IsFinishAnim())
		{
			m_animIndex = 2;
			m_modelCom->Set_Animation(m_animIndex, false);
			m_state = EState::RELEASE;
			m_handCollider1->Set_CollisionActive(false);
			m_handCollider2->Set_CollisionActive(false);
			m_gameInstance.lock()->Play_Once(L"WreckMonsterRetreat");

		}
	}
	else if(m_state == RELEASE && m_modelCom->Get_IsFinishAnim() == true)
	{
		

		m_animIndex = 1;
		m_modelCom->Set_Animation(m_animIndex, true);
		m_state = EState::IDLE;
	}

	m_colliderCom->Update(XMLoadFloat4x4(m_transformCom->Get_WorldMatrixPtr()));


	m_handCollider1->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_socketMatrixRightHand)));
	m_handCollider2->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_socketMatrixLeftHand)));
	
}

void CMonster::Late_Update(_float timeDelta)
{


	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMonster::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMesh = m_modelCom->Get_NumMeshes();

	for(size_t i=0; i< numMesh; i++)
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
	if (m_handCollider1->Get_CollisionActive())
		m_gameInstance.lock()->Add_DebugenderGroup(m_handCollider1);

	if (m_handCollider2->Get_CollisionActive())
		m_gameInstance.lock()->Add_DebugenderGroup(m_handCollider2);

	if (m_colliderCom->Get_CollisionActive())
		m_gameInstance.lock()->Add_DebugenderGroup(m_colliderCom);

#endif


	return S_OK;
}

void CMonster::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	if(m_state != EState::ATTACK &&  self == m_colliderCom)
	{

		m_gameInstance.lock()->Play_Once(L"WreckMonsterEmerge");

	m_animIndex = 0;

	m_modelCom->Set_Animation(m_animIndex, false);
	m_state = ATTACK;


	
	}

	if(m_state == ATTACK)
	{
		if (self == m_handCollider2 || self == m_handCollider1)
		{
			dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner())->Get_Damage();
		
		}
	}



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

	m_textureCom = Get_Component<CTexture>(L"Com_Texture");
	m_modelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CMonster::Bind_ShaderResources()
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


	float farZ = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far", &farZ, sizeof(_float))))
		return E_FAIL;
		
	float emissive =0.f;
	if (FAILED(m_shaderCom->Bind_RawValue("g_EmissiveStrength", &emissive, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMonster::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_FullBoatCrab"), TEXT("Com_Model"), &m_modelCom, nullptr)))
		return E_FAIL;


	CBounding_OBB::BoundingOBBDesc		OBBDesc{};
	OBBDesc.extents = _float3(3.39f, 1.f, 2.f);
	OBBDesc.radians = _float3(0.f, 0.f, 0.f);
	OBBDesc.center = _float3(0.f, 0.f, 2.333f);
	OBBDesc.myLayer = ECollisionLayer::TRIGGER;
	OBBDesc.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_colliderCom, &OBBDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_colliderCom);

	CBounding_Sphere::BoundingSphereDesc		sphereDesc{};
	
	sphereDesc.radius = 2.f;
	sphereDesc.center = _float3(0.f, 2.f,0.f);
	sphereDesc.myLayer = ECollisionLayer::MONSTERATT;
	sphereDesc.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Hand_Collider_1"), &m_handCollider1 , &sphereDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_handCollider1);


	CBounding_Sphere::BoundingSphereDesc		sphereDesc1{};
	sphereDesc1.radius = 2.f;
	sphereDesc1.center = _float3(0.f, -2.f, 0.f);
	sphereDesc1.myLayer = ECollisionLayer::MONSTERATT;
	sphereDesc1.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(__super::Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Hand_Collider_2"), &m_handCollider2, &sphereDesc1)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_handCollider2);



	return S_OK;
}

shared_ptr<CMonster> CMonster::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CMonster> instance(new CMonster(device, context), [](CMonster* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
	}
	return instance;
}


shared_ptr<CGameObject> CMonster::Clone(void* arg)
{
	shared_ptr<CMonster> instance(new CMonster(*this), [](CMonster* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
	}
	return instance;
}

void CMonster::Free()
{
	__super::Free();
}
