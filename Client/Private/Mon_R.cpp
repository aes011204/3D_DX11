#include "Mon_R.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"


CMon_R::CMon_R(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
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

HRESULT CMon_R::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_pPlayer = m_pGameInstance.lock()->Get_GameObject(
		ETOI(LEVEL::GAMEPLAY),
		L"Layer_Player", 0
	);


	if (m_pPlayer.lock())
	{
	_float3 newPos = {};
	_vector TargetPos = m_pPlayer.lock()->Get_TransformCom()->Get_State(STATE::LOOK) * 100.f;

		//XMVectorSet(targetPos.x, targetPos.y, targetPos.z + 20.f, 1.f)
	m_pTransformCom->Set_Position(m_pPlayer.lock()->Get_TransformCom()->Get_Position() + TargetPos);


	m_pTransformCom->SetUp_Scale(2.f, 2.f, 2.f);
	}


	m_AnimIndex = 1;
	m_pModelCom_One->Set_Animation(m_AnimIndex, false);



	m_IndexETC = 4;
	m_pModelCom_Etc->Set_Animation(m_IndexETC, true);
	
	



	return S_OK;
}

void CMon_R::Priority_Update(_float fTimeDelta)
{
}

void CMon_R::Update(_float fTimeDelta)
{
	m_pModelCom_One->Play_Animation(fTimeDelta);



		m_pModelCom_Etc->Play_Animation(fTimeDelta);

	
	if(m_pModelCom_One->Get_IsFinishAnim() == true)
	{
		if(m_Count == 1)
		{

			if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_R_Act"),
				ETOI(LEVEL::GAMEPLAY), L"Layer_Moster")))
				return;
		}
		++m_Count;

		
	m_pModelCom_One->Set_Animation(m_AnimIndex, false);
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

void CMon_R::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMon_R::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	{
		size_t iNumMesh = m_pModelCom_One->Get_NumMeshes();

		for (size_t i = 0; i < iNumMesh; i++)
		{
			m_pModelCom_One->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
			m_pModelCom_One->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom_One->Render(i)))
				return E_FAIL;
		}
	}
	//
	{
		/*for (int j =8; j < 11; j++)
		{*/
			size_t iNumMesh = m_pModelCom_Etc->Get_NumMeshes();
		
			for (size_t i = 0; i < iNumMesh; i++)
			{
				m_pModelCom_Etc->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
				m_pModelCom_Etc->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
		
				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;
		
				if (FAILED(m_pModelCom_Etc->Render(i)))
					return E_FAIL;
			}
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
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
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

HRESULT CMon_R::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_One"), TEXT("Com_Model_One"), &m_pModelCom_One, nullptr)))
		return E_FAIL;

	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함
	

		if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_R_Act_Etc"), TEXT("Com_Model_Etc"), &m_pModelCom_Etc, nullptr)))
			return E_FAIL;



	return S_OK;
}

shared_ptr<CMon_R> CMon_R::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CMon_R> pInstance(new CMon_R(pDevice, pContext), [](CMon_R* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMon_R");
	}
	return pInstance;
}


shared_ptr<CGameObject> CMon_R::Clone(void* pArg)
{
	shared_ptr<CMon_R> pInstance(new CMon_R(*this), [](CMon_R* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMon_R");
	}
	return pInstance;
}

void CMon_R::Free()
{
	__super::Free();
}
