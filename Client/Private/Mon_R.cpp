#include "Mon_R.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"


CMon_R::CMon_R(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject{ device ,context }
{
}

CMon_R::CMon_R(const CMon_R& prototype)
	: CGameObject{prototype}
{
}

HRESULT CMon_R::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMon_R::Initialize(void* arg)
{
	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_player = m_gameInstance.lock()->Get_GameObject(
		ETOI(ELevel::GAMEPLAY),
		L"Layer_Player", 0
	);


	if (m_player.lock())
	{
	_float3 newPos = {};
	_vector targetPos = m_player.lock()->Get_TransformCom()->Get_State(ETransformState::LOOK) * 100.f;

		//XMVectorSet(targetPos.x, targetPos.y, targetPos.z + 20.f, 1.f)
	m_transformCom->Set_Position(m_player.lock()->Get_TransformCom()->Get_Position() + targetPos);


	m_transformCom->SetUp_Scale(2.f, 2.f, 2.f);
	}


	m_animIndex = 1;
	m_modelComOne->Set_Animation(m_animIndex, false);



	m_indexEtc = 4;
	m_modelComEtc->Set_Animation(m_indexEtc, true);
	
	

	m_gameInstance.lock()->Play_Loop(L"Leviathan_Rumble_Loop");

	m_gameInstance.lock()->Play_Loop(L"Leviathan_Distant_Call");

	return S_OK;
}

void CMon_R::Priority_Update(_float timeDelta)
{
}

void CMon_R::Update(_float timeDelta)
{
	m_modelComOne->Play_Animation(timeDelta);



		m_modelComEtc->Play_Animation(timeDelta);

	
	if(m_modelComOne->Get_IsFinishAnim() == true)
	{
		++m_count;
		if(m_count == 1)
		{

			if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_R_Act"),
				ETOI(ELevel::GAMEPLAY), L"Layer_Moster")))
				return;
		}

		
	m_modelComOne->Set_Animation(m_animIndex, false);
	}


	//m_pModelCom_Etc->Play_Animation(fTimeDelta);

	//if(m_State == ATTACK && m_pModelCom->Get_IsFinishAnim()==true)
	//{
	//	m_AnimIndex = 2;
	//	m_pModelCom->Set_Animation(m_AnimIndex, false);
	//	m_State = STATE::RELEASE;
	//}
	//else if(m_State == RELEASE && m_pModelCom->Get_IsFinishAnim() == true)
	//{
	//	m_AnimIndex = 1;
	//	m_pModelCom->Set_Animation(m_AnimIndex, true);
	//	m_State = STATE::IDLE;
	//}

	//m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


	//m_Hand_Collider_1->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_RightHand)));
	//m_Hand_Collider_2->Update(CombinedWorldMatrix(XMLoadFloat4x4(m_pSocketMatrix_LefttHand)));
	
}

void CMon_R::Late_Update(_float timeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMon_R::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	{
		size_t numMesh = m_modelComOne->Get_NumMeshes();

		for (size_t i = 0; i < numMesh; i++)
		{
			m_modelComOne->Bind_Material(m_shaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);

			m_modelComOne->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
			m_modelComOne->Bind_BoneMatrices(m_shaderCom, "g_BoneMatrices", i);

			if (FAILED(m_shaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_modelComOne->Render(i)))
				return E_FAIL;
		}
	}
	//
	{
		/*for (int j =8; j < 11; j++)
		{*/
			size_t numMesh = m_modelComEtc->Get_NumMeshes();
		
			//for (size_t i = 0; i < iNumMesh; i++)
			//{
			//	//m_pModelCom_Etc->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);
			//	//
			//	//m_pModelCom_Etc->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
			//	//m_pModelCom_Etc->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
			//	//
			//	//if (FAILED(m_pShaderCom->Begin(0)))
			//	//	return E_FAIL;
			//
			//	//if (FAILED(m_pModelCom_Etc->Render(i)))
			//	//	return E_FAIL;
			//}
		/*}*/


		//size_t iNumMesh = m_pModelCom_Etc->Get_NumMeshes();
		//
		//for (size_t i = 0; i < iNumMesh; i++)
		//{
		//	m_pModelCom_Etc->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		//	m_pModelCom_Etc->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
		//
		//	if (FAILED(m_pShaderCom->Begin(0)))
		//		return E_FAIL;
		//
		//	if (FAILED(m_pModelCom_Etc->Render(i)))
		//		return E_FAIL;
		//}
	}

#ifdef _DEBUG
	if (m_gameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	//m_pColliderCom->Render();

	//m_Hand_Collider_1->Render();
	//m_Hand_Collider_2->Render();

#endif




	return S_OK;
}

void CMon_R::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	//if(m_State != STATE::ATTACK &&  self == m_pColliderCom)
	//{
	//m_AnimIndex = 0;

	//m_pModelCom->Set_Animation(m_AnimIndex, false);
	//m_State = ATTACK;
	//
	//}

	//if(m_State == ATTACK)
	//{
	//	/*if (self == m_Hand_Collider_2 || self == m_Hand_Collider_1)
	//	{
	//		dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner())->Get_Demage();
	//	
	//	}*/
	//}



}
void CMon_R::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{


	int i = 0;

	
}

void CMon_R::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	int i = 0;

}

void CMon_R::OnGui()
{
	//ImGui::SliderInt("ETC Anim Index", (_int*)&m_IndexETC, 0, 10);

	//// 버튼으로도 테스트
	//if (ImGui::Button("Prev"))
	//{
	//	if (m_IndexETC > 0)
	//		m_IndexETC--;
	//	m_pModelCom_Etc->Set_Animation(m_IndexETC, true);
	//}

	//ImGui::SameLine();

	//if (ImGui::Button("Next"))
	//{
	//	if (m_IndexETC < 10)
	//		m_IndexETC++;
	//	m_pModelCom_Etc->Set_Animation(m_IndexETC, true);
	//}

	//// 현재 값 출력
	//ImGui::Text("Current Index: %d", m_IndexETC);

	


}

void CMon_R::RebindCom()
{

	//m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	//m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CMon_R::Bind_ShaderResources()
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
	return S_OK;
}

HRESULT CMon_R::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_One"), TEXT("Com_Model_One"), &m_modelComOne, nullptr)))
		return E_FAIL;

	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함
	

		if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_Etc"), TEXT("Com_Model_Etc"), &m_modelComEtc, nullptr)))
			return E_FAIL;



	return S_OK;
}

shared_ptr<CMon_R> CMon_R::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CMon_R> instance(new CMon_R(device, context), [](CMon_R* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMon_R");
	}
	return instance;
}


shared_ptr<CGameObject> CMon_R::Clone(void* arg)
{
	shared_ptr<CMon_R> instance(new CMon_R(*this), [](CMon_R* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CMon_R");
	}
	return instance;
}

void CMon_R::Free()
{
	__super::Free();
}
