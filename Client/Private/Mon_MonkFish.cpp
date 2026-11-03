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



	m_lengthNear = 13.f;
	m_lengthAttack = 6.f;
	m_alphaMesh = 0;
	m_alphaAnim = 0;
	m_alphaSpeed = 4.f;

	m_player = m_gameInstance.lock()->Get_GameObject(
		ETOI(ELevel::GAMEPLAY),
		L"Layer_Player", 0
	);

	_float3 targetPos = {};
	XMStoreFloat3(&targetPos, m_player.lock()->Get_TransformCom()->Get_Position());

	_vector finalPos = XMVectorSet(targetPos.x + 40.f, 0.f, targetPos.z + 30.f, 1.f);

	m_transformCom->Set_Position(finalPos);

	m_animIndex = 2;
	m_modelComMon->Set_Animation(m_animIndex, true);


	ChangeState(EState::REVEAL);
	m_transformCom->Set_Speed(2.f);

	m_socketMatrix = m_modelComMon->Get_BoneMatrixPtr("jaw2_jnt");

	m_socketMatrixLight = m_modelComMon->Get_BoneMatrixPtr("feeler6_jnt");


	LightDesc lightDesc = {};

	lightDesc.type = ELightType::POINT;
	lightDesc.position = _float4(0.f, 0.2f, 0.f, 1.f);
	lightDesc.range = 5.f;
	lightDesc.diffuse = _float4(2.f, 1.8f, 1.4f, 1.f);
	lightDesc.ambient = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.specular = _float4(1.f, 1.f, 1.f, 1.f);

	m_lightObj = (m_gameInstance.lock()->Add_Light(lightDesc));
	if (m_lightObj == nullptr)
		return E_FAIL;


	return S_OK;
}

void CMon_MonkFish::Priority_Update(_float fTimeDelta)
{
}

CMon_MonkFish::EDistState CMon_MonkFish::GetDistanceState(_float dist) const
{
	if (dist <= m_lengthAttack)
		return EDistState::ATTACK_RANGE;

	if (dist <= m_lengthNear)
		return EDistState::E_NEAR;

	return EDistState::E_FAR;
}

void CMon_MonkFish::UpdateBlendByDistanceState(EDistState distState, _float fTimeDelta)
{
	if (distState == EDistState::E_FAR)
	{
		m_alphaMesh += fTimeDelta * m_alphaSpeed;
		m_alphaAnim -= fTimeDelta * m_alphaSpeed;
	}
	else
	{
		m_alphaMesh -= fTimeDelta * m_alphaSpeed;
		m_alphaAnim += fTimeDelta * m_alphaSpeed;
	}

	m_alphaMesh = clamp(m_alphaMesh, 0.f, 1.f);
	m_alphaAnim = clamp(m_alphaAnim, 0.f, 1.f);
}

void CMon_MonkFish::Update(_float fTimeDelta)
{
	m_modelComMon->Play_Animation(fTimeDelta);

	if (IsColl == true)
	{
		if(m_modelComMon->Get_IsFinishAnim())
		{
			ChangeState(EState::RUNAWAY);
			IsColl = false;
		}
		
	}

	_vector vToPlayer = m_player.lock()->Get_TransformCom()->Get_Position() - m_transformCom->Get_Position();
	_float dist = XMVectorGetX(XMVector3Length(vToPlayer));
	EDistState distState = GetDistanceState(dist);

	_int curFrame = m_modelComMon->Get_CurrentFrame();


	switch (m_state)
	{
	case EState::ATTACK:
		if (distState != EDistState::ATTACK_RANGE)
		{
			ChangeState(EState::P_NEAR);
		}
		if (dist >= 2.f)
		{
			m_transformCom->LookAt(m_player.lock()->Get_TransformCom()->Get_Position());
			m_transformCom->Go_Forward(fTimeDelta);
		}
		if (m_attackPrevFrame < 80 && curFrame >= 80)
		{

		}
		break;
	case EState::IDLE:
		if (distState != EDistState::E_FAR)
		{
			ChangeState(EState::P_NEAR);
		}
		if (dist >= 2.f)
		{
			m_transformCom->LookAt(m_player.lock()->Get_TransformCom()->Get_Position());
			m_transformCom->Go_Forward(fTimeDelta);
		}

		break;
	case EState::P_NEAR:
		if (distState == EDistState::ATTACK_RANGE)
		{
			ChangeState(EState::ATTACK);
		}
		else if (distState == EDistState::E_FAR)
		{
			ChangeState(EState::IDLE);

		}
		if (dist >= 2.f)
		{
			m_transformCom->LookAt(m_player.lock()->Get_TransformCom()->Get_Position());
			m_transformCom->Go_Forward(fTimeDelta);
		}
		break;
	case EState::REVEAL:
		m_alphaMesh += fTimeDelta * m_alphaSpeed*0.3f;
		if (m_alphaMesh >= 1)
		{
			m_alphaMesh = 1;
			/*	if (m_pModelCom_Mon->Get_IsFinishAnim() == true)
				{*/
			ChangeState(EState::IDLE);

		}
		if (dist >= 2.f)
		{
			m_transformCom->LookAt(m_player.lock()->Get_TransformCom()->Get_Position());
			m_transformCom->Go_Forward(fTimeDelta);
		}
		break;

	case EState::RUNAWAY:
		//m_pTransformCom->LookAt(XMLoadFloat3(&m_Dir));
		m_transformCom->Go_Forward(fTimeDelta);



		break;


	}
		m_attackPrevFrame = curFrame;
	UpdateBlendByDistanceState(distState, fTimeDelta);

	XMMATRIX final = CombinedWorldMatrix(XMLoadFloat4x4(m_socketMatrixLight));

	_float3 pos;
	XMStoreFloat3(&pos, final.r[3]);

	m_lightObj->Set_Position(pos.x, pos.y+0.5f, pos.z);




	m_colliderCom->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_socketMatrix)));


}
void CMon_MonkFish::ChangeState(EState newState)
{
	if (m_state != newState)
	{
		EnterState(newState);
		m_prevState = m_state;
		m_state = newState;
	}
}
void CMon_MonkFish::EnterState(EState newState)
{



	switch (newState)
	{
	case EState::ATTACK:
		m_gameInstance.lock()->Play_Once(L"MarrowMonster_Attack");

		m_modelComMon->Set_Animation(1, true);
		break;
	case EState::IDLE:

		m_modelComMon->Set_Animation(2, true);
		break;
	case EState::P_NEAR:
		m_gameInstance.lock()->Stop(L"MarrowMonster_IdleLoop");
		m_gameInstance.lock()->Play_Loop(L"MarrowMonster_AggroLoop");
		m_gameInstance.lock()->Play_Once(L"MarrowMonster_Call");
		m_modelComMon->Set_Animation(0, true);
		break;
	case EState::REVEAL:

		m_modelComMon->Set_Animation(2, true);
		m_gameInstance.lock()->Play_Once(L"MarrowMonster_Call");
			m_gameInstance.lock()->Play_Loop(L"MarrowMonster_IdleLoop");

		break;
	case EState::RUNAWAY:
		m_transformCom->Set_Speed(5.f);
		m_modelComMon->Set_Animation(0, true);

		m_gameInstance.lock()->Play_Once(L"MarrowMonster_Retreat");

		break;
	}

	if(m_gameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_L)==true)
	{
		Mark_Destroy();
	}



}

void CMon_MonkFish::Late_Update(_float fTimeDelta)
{

	if (m_alphaAnim < 1.f - 0.001f)
	{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::BLEND, static_pointer_cast<CMon_MonkFish>(shared_from_this()));
		
	}
	else
	{
		m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CMon_MonkFish>(shared_from_this()));

	}
}

HRESULT CMon_MonkFish::Render()
{



	if (m_alphaAnim > 0.f)
	{

		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		size_t iNumMesh = m_modelComMon->Get_NumMeshes();

		for (size_t i = 0; i < iNumMesh; i++)
		{
			m_modelComMon->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);

			m_modelComMon->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
			m_modelComMon->Bind_BoneMatrices(m_shaderCom, "g_BoneMatrices", i);


			if (m_alphaAnim < 1.f)
				m_shaderCom->Begin(1); // Alpha
			else
				m_shaderCom->Begin(0); // Opaque

			if (FAILED(m_modelComMon->Render(i)))
				return E_FAIL;

		}
	}

	if (m_alphaMesh > 0.f)
	{

		size_t iNumMesh = m_modelComBoat->Get_NumMeshes();

			if (FAILED(Bind_ShaderResources_Mesh()))
				return E_FAIL;

		for (size_t j = 0; j < iNumMesh; j++)
		{
			m_modelComBoat->Bind_Material(m_shaderComMesh, "g_DiffuseTexture", j, TextureType_DIFFUSE, 0);
			m_modelComBoat->Bind_Material(m_shaderCom, "g_EmissiveTexture", j, TextureType_EMISSIVE, 0);


			if (FAILED(m_shaderComMesh->Begin(1)))
				return E_FAIL;

			if (FAILED(m_modelComBoat->Render(j)))
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

void CMon_MonkFish::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{

	_vector vToPlayer = m_player.lock()->Get_TransformCom()->Get_Position() - m_transformCom->Get_Position();
	_vector dir = XMVector3Normalize(m_player.lock()->Get_TransformCom()->Get_Position() - m_transformCom->Get_Position());
	
	_vector targetPos = m_transformCom->Get_Position() + vToPlayer * 100.f;
	//IsColl = true;
	XMStoreFloat3(&m_dir, targetPos);





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

	if (ImGui::Button("Idle"))   ChangeState(EState::IDLE);
	if (ImGui::Button("REVEAL"))   ChangeState(EState::REVEAL);
	if (ImGui::Button("Attack")) ChangeState(EState::ATTACK);
	if (ImGui::Button("P_NEAR"))    ChangeState(EState::P_NEAR);

	ImGui::Text("Current State: %s",
		magic_enum::enum_name(m_state).data());

	_vector vToPlayer = m_player.lock()->Get_TransformCom()->Get_Position() - m_transformCom->Get_Position();
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
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
		return E_FAIL;


	if (FAILED(m_shaderCom->Bind_RawValue("g_Alpha", &m_alphaAnim, sizeof(_float))))
		return E_FAIL;

	float fFar = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far", &fFar, sizeof(_float))))
		return E_FAIL;

	float emissive = 4.f;
	if (FAILED(m_shaderCom->Bind_RawValue("g_EmissiveStrength", &emissive, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMon_MonkFish::Bind_ShaderResources_Mesh()
{
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderComMesh, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderComMesh, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderComMesh, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderComMesh, "g_vCamPosition")))
		return E_FAIL;


	if (FAILED(m_shaderComMesh->Bind_RawValue("g_Alpha", &m_alphaMesh, sizeof(_float))))
		return E_FAIL;



	return S_OK;
}

_vector CMon_MonkFish::Get_WorldPos()
{
	{
		return m_transformCom->Get_Position();
	}
}


HRESULT CMon_MonkFish::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"), &m_shaderComMesh, nullptr)))
		return E_FAIL;


	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Mon"), TEXT("Com_Model_Mon"), &m_modelComMon, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Marrow_Boat"), TEXT("Com_Model_Boat"), &m_modelComBoat, nullptr)))
		return E_FAIL;


	CBounding_AABB::BoundingAABBDesc		AABBDesc{};
	AABBDesc.extents = _float3(10000.f, 900.f, 10000.f);
	AABBDesc.center = _float3(0.f, 3.f, 0.f);
	AABBDesc.myLayer = ECollisionLayer::MONSTERATT;
	AABBDesc.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &m_colliderCom, &AABBDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_colliderCom);



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

