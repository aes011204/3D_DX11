#include "Mon_MonkFish.h"

#include "DInput_Manager.h"

#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"


CMon_MonkFish::CMon_MonkFish(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CMon_MonkFish::CMon_MonkFish(const CMon_MonkFish& prototype)
	: CGameObject{ prototype }
{
}

HRESULT CMon_MonkFish::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMon_MonkFish::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_LenghtNear = 13.f;
	m_LenghtAttack = 6.f;
	m_Alpha_Mesh = 0;
	m_Alpha_Anim = 0;
	m_AlphaSpeed = 4.f;

	m_pPlayer = m_pGameInstance.lock()->Get_GameObject(
		ETOI(LEVEL::GAMEPLAY),
		L"Layer_Player", 0
	);

	_float3 targetPos = {};
	XMStoreFloat3(&targetPos, m_pPlayer.lock()->Get_TransformCom()->Get_Position());

	_vector finalPos = XMVectorSet(targetPos.x + 40.f, 0.f, targetPos.z + 30.f, 1.f);

	m_pTransformCom->Set_Position(finalPos);

	m_AnimIndex = 2;
	m_pModelCom_Mon->Set_Animation(m_AnimIndex, true);


	ChangeState(STATE::REVEAL);
	m_pTransformCom->Set_Speed(2.f);

	m_pSocketMatrix = m_pModelCom_Mon->Get_BoneMatrixPtr("jaw2_jnt");

	m_pSocketMatrix_Light = m_pModelCom_Mon->Get_BoneMatrixPtr("feeler6_jnt");


	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vPosition = _float4(0.f, 0.2f, 0.f, 1.f);
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(2.f, 1.8f, 1.4f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_LightObj = (m_pGameInstance.lock()->Add_Light(LightDesc));
	if (m_LightObj == nullptr)
		return E_FAIL;


	return S_OK;
}

void CMon_MonkFish::Priority_Update(_float fTimeDelta)
{
}

CMon_MonkFish::DIST_STATE CMon_MonkFish::GetDistanceState(_float dist) const
{
	if (dist <= m_LenghtAttack)
		return DIST_STATE::ATTACK_RANGE;

	if (dist <= m_LenghtNear)
		return DIST_STATE::E_NEAR;

	return DIST_STATE::E_FAR;
}

void CMon_MonkFish::UpdateBlendByDistanceState(DIST_STATE distState, _float fTimeDelta)
{
	if (distState == DIST_STATE::E_FAR)
	{
		m_Alpha_Mesh += fTimeDelta * m_AlphaSpeed;
		m_Alpha_Anim -= fTimeDelta * m_AlphaSpeed;
	}
	else
	{
		m_Alpha_Mesh -= fTimeDelta * m_AlphaSpeed;
		m_Alpha_Anim += fTimeDelta * m_AlphaSpeed;
	}

	m_Alpha_Mesh = clamp(m_Alpha_Mesh, 0.f, 1.f);
	m_Alpha_Anim = clamp(m_Alpha_Anim, 0.f, 1.f);
}

void CMon_MonkFish::Update(_float fTimeDelta)
{
	m_pModelCom_Mon->Play_Animation(fTimeDelta);

	if (IsColl == true)
	{
		if(m_pModelCom_Mon->Get_IsFinishAnim())
		{
			ChangeState(STATE::RUNAWAY);
			IsColl = false;
		}
		
	}

	_vector vToPlayer = m_pPlayer.lock()->Get_TransformCom()->Get_Position() - m_pTransformCom->Get_Position();
	_float dist = XMVectorGetX(XMVector3Length(vToPlayer));
	DIST_STATE distState = GetDistanceState(dist);

	_int curFrame = m_pModelCom_Mon->Get_CurrentFrame();


	switch (m_State)
	{
	case STATE::ATTACK:
		if (distState != DIST_STATE::ATTACK_RANGE)
		{
			ChangeState(STATE::P_NEAR);
		}
		if (dist >= 2.f)
		{
			m_pTransformCom->LookAt(m_pPlayer.lock()->Get_TransformCom()->Get_Position());
			m_pTransformCom->Go_Forward(fTimeDelta);
		}
		if (m_iAttackPrevFrame < 80 && curFrame >= 80)
		{

		}
		break;
	case STATE::IDLE:
		if (distState != DIST_STATE::E_FAR)
		{
			ChangeState(STATE::P_NEAR);
		}
		if (dist >= 2.f)
		{
			m_pTransformCom->LookAt(m_pPlayer.lock()->Get_TransformCom()->Get_Position());
			m_pTransformCom->Go_Forward(fTimeDelta);
		}

		break;
	case STATE::P_NEAR:
		if (distState == DIST_STATE::ATTACK_RANGE)
		{
			ChangeState(STATE::ATTACK);
		}
		else if (distState == DIST_STATE::E_FAR)
		{
			ChangeState(STATE::IDLE);

		}
		if (dist >= 2.f)
		{
			m_pTransformCom->LookAt(m_pPlayer.lock()->Get_TransformCom()->Get_Position());
			m_pTransformCom->Go_Forward(fTimeDelta);
		}
		break;
	case STATE::REVEAL:
		m_Alpha_Mesh += fTimeDelta * m_AlphaSpeed*0.3f;
		if (m_Alpha_Mesh >= 1)
		{
			m_Alpha_Mesh = 1;
			/*	if (m_pModelCom_Mon->Get_IsFinishAnim() == true)
				{*/
			ChangeState(STATE::IDLE);

		}
		if (dist >= 2.f)
		{
			m_pTransformCom->LookAt(m_pPlayer.lock()->Get_TransformCom()->Get_Position());
			m_pTransformCom->Go_Forward(fTimeDelta);
		}
		break;

	case STATE::RUNAWAY:
		//m_pTransformCom->LookAt(XMLoadFloat3(&m_Dir));
		m_pTransformCom->Go_Forward(fTimeDelta);



		break;


	}
		m_iAttackPrevFrame = curFrame;
	UpdateBlendByDistanceState(distState, fTimeDelta);

	XMMATRIX final = CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_Light));

	_float3 pos;
	XMStoreFloat3(&pos, final.r[3]);

	m_LightObj->Set_Position(pos.x, pos.y+0.5f, pos.z);




	m_pColliderCom->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix)));


}
void CMon_MonkFish::ChangeState(STATE newState)
{
	if (m_State != newState)
	{
		EnterState(newState);
		m_PrevState = m_State;
		m_State = newState;
	}
}
void CMon_MonkFish::EnterState(STATE newState)
{



	switch (newState)
	{
	case STATE::ATTACK:
		m_pGameInstance.lock()->Play_Once(L"MarrowMonster_Attack");

		m_pModelCom_Mon->Set_Animation(1, true);
		break;
	case STATE::IDLE:

		m_pModelCom_Mon->Set_Animation(2, true);
		break;
	case STATE::P_NEAR:
		m_pGameInstance.lock()->Stop(L"MarrowMonster_IdleLoop");
		m_pGameInstance.lock()->Play_Loop(L"MarrowMonster_AggroLoop");
		m_pGameInstance.lock()->Play_Once(L"MarrowMonster_Call");
		m_pModelCom_Mon->Set_Animation(0, true);
		break;
	case STATE::REVEAL:

		m_pModelCom_Mon->Set_Animation(2, true);
		m_pGameInstance.lock()->Play_Once(L"MarrowMonster_Call");
			m_pGameInstance.lock()->Play_Loop(L"MarrowMonster_IdleLoop");

		break;
	case STATE::RUNAWAY:
		m_pTransformCom->Set_Speed(5.f);
		m_pModelCom_Mon->Set_Animation(0, true);

		m_pGameInstance.lock()->Play_Once(L"MarrowMonster_Retreat");

		break;
	}

	if(m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_L)==true)
	{
		Mark_Destroy();
	}



}

void CMon_MonkFish::Late_Update(_float fTimeDelta)
{

	if (m_Alpha_Anim < 1.f - 0.001f)
	{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::BLEND, static_pointer_cast<CMon_MonkFish>(shared_from_this()));
		
	}
	else
	{
		m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CMon_MonkFish>(shared_from_this()));

	}
}

HRESULT CMon_MonkFish::Render()
{



	if (m_Alpha_Anim > 0.f)
	{

		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		size_t iNumMesh = m_pModelCom_Mon->Get_NumMeshes();

		for (size_t i = 0; i < iNumMesh; i++)
		{
			m_pModelCom_Mon->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);

			m_pModelCom_Mon->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
			m_pModelCom_Mon->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);


			if (m_Alpha_Anim < 1.f)
				m_pShaderCom->Begin(1); // Alpha
			else
				m_pShaderCom->Begin(0); // Opaque

			if (FAILED(m_pModelCom_Mon->Render(i)))
				return E_FAIL;

		}
	}

	if (m_Alpha_Mesh > 0.f)
	{

		size_t iNumMesh = m_pModelCom_Boat->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources_Mesh()))
				return E_FAIL;

		for (size_t j = 0; j < iNumMesh; j++)
		{
			m_pModelCom_Boat->Bind_Material(m_pShaderCom_Mesh, "g_DiffuseTexture", j, TextureType_DIFFUSE, 0);
			m_pModelCom_Boat->Bind_Material(m_pShaderCom, "g_EmissiveTexture", j, TextureType_EMISSIVE, 0);


			if (FAILED(m_pShaderCom_Mesh->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pModelCom_Boat->Render(j)))
				return E_FAIL;
		}

	}

#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_pGameInstance.lock()->Add_DebugenderGroup(m_pColliderCom);
#endif



	return S_OK;
}

void CMon_MonkFish::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{

	_vector vToPlayer = m_pPlayer.lock()->Get_TransformCom()->Get_Position() - m_pTransformCom->Get_Position();
	_vector dir = XMVector3Normalize(m_pPlayer.lock()->Get_TransformCom()->Get_Position() - m_pTransformCom->Get_Position());
	
	_vector targetPos = m_pTransformCom->Get_Position() + vToPlayer * 100.f;
	//IsColl = true;
	XMStoreFloat3(&m_Dir, targetPos);





}
void CMon_MonkFish::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{


	int i = 0;


}

void CMon_MonkFish::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	int i = 0;

}

void CMon_MonkFish::OnGui()
{

	if (ImGui::Button("Idle"))   ChangeState(STATE::IDLE);
	if (ImGui::Button("REVEAL"))   ChangeState(STATE::REVEAL);
	if (ImGui::Button("Attack")) ChangeState(STATE::ATTACK);
	if (ImGui::Button("P_NEAR"))    ChangeState(STATE::P_NEAR);

	ImGui::Text("Current State: %s",
		magic_enum::enum_name(m_State).data());

	_vector vToPlayer = m_pPlayer.lock()->Get_TransformCom()->Get_Position() - m_pTransformCom->Get_Position();
	_float dist = XMVectorGetX(XMVector3Length(vToPlayer));
	ImGui::Text("Distance: %f", dist);
	ImGui::Text("Distance State: %s", magic_enum::enum_name(GetDistanceState(dist)).data());
}

void CMon_MonkFish::RebindCom()
{

	/*m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");*/

}

HRESULT CMon_MonkFish::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_Alpha_Anim, sizeof(_float))))
		return E_FAIL;

	float fFar = m_pGameInstance.lock()->Get_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Far", &fFar, sizeof(_float))))
		return E_FAIL;

	float emissive = 4.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EmissiveStrength", &emissive, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMon_MonkFish::Bind_ShaderResources_Mesh()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom_Mesh, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom_Mesh, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom_Mesh, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom_Mesh, "g_vCamPosition")))
		return E_FAIL;


	if (FAILED(m_pShaderCom_Mesh->Bind_RawValue("g_Alpha", &m_Alpha_Mesh, sizeof(_float))))
		return E_FAIL;



	return S_OK;
}

_vector CMon_MonkFish::Get_WorldPos()
{
	{
		return m_pTransformCom->Get_Position();
	}
}


HRESULT CMon_MonkFish::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"), &m_pShaderCom_Mesh, nullptr)))
		return E_FAIL;


	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Mon"), TEXT("Com_Model_Mon"), &m_pModelCom_Mon, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Boat"), TEXT("Com_Model_Boat"), &m_pModelCom_Boat, nullptr)))
		return E_FAIL;


	CBounding_AABB::BOUNDING_AABB_DESC		AABBDesc{};
	AABBDesc.vExtents = _float3(10000.f, 900.f, 10000.f);
	AABBDesc.vCenter = _float3(0.f, 3.f, 0.f);
	AABBDesc.MyLayer = COLLISION_LAYER::MONSTERATT;
	AABBDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &m_pColliderCom, &AABBDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_pColliderCom);



	return S_OK;
}

shared_ptr<CMon_MonkFish> CMon_MonkFish::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CMon_MonkFish> pInstance(new CMon_MonkFish(pDevice, pContext), [](CMon_MonkFish* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMon_MonkFish");
	}
	return pInstance;
}


shared_ptr<CGameObject> CMon_MonkFish::Clone(void* pArg)
{
	shared_ptr<CMon_MonkFish> pInstance(new CMon_MonkFish(*this), [](CMon_MonkFish* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMon_MonkFish");
	}
	return pInstance;
}

void CMon_MonkFish::Free()
{
	__super::Free();
}

