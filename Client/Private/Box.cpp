#include "Box.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"
#include "EventBus.h"
#include "Client_Enum.h"
#include "Inventory.h"


CBox::CBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CBox::CBox(const CBox& prototype)
	: CGameObject{prototype}
{
}

HRESULT CBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_animIndex = 1;
	m_modelCom->Set_Animation(m_animIndex, true);
	m_state = EState::IDLE;


	{
		Evt_InvenStorageInitData e = {};
		e.invenPtr = m_storageCom;
		e.invenType = m_storageCom->Get_Inventype();
		m_gameInstance.lock()->Get_EventBus()->Publish<Evt_InvenStorageInitData>(e);
	}
	_float3 pos = _float3(49.5f, -0.4f, -19.f);
	m_transformCom->Set_Position(XMLoadFloat3(&pos));
	m_transformCom->Set_RotationDegree(_float3(9.5f, 8.9f, 0.f));
	m_transformCom->Update_WorldMatrix();
	
	return S_OK;
}

void CBox::Priority_Update(_float fTimeDelta)
{
}

void CBox::Update(_float fTimeDelta)
{
	m_modelCom->Play_Animation(fTimeDelta);


	if(m_modelCom->Get_IsFinishAnim() == true)
	{
		int i = 0;
	}

	if(m_state == ATTACK && m_modelCom->Get_IsFinishAnim()==true)
	{
		m_animIndex = 2;
		m_modelCom->Set_Animation(m_animIndex, false);
		m_state = EState::RELEASE;
	}
	else if(m_state == RELEASE && m_modelCom->Get_IsFinishAnim() == true)
	{
		m_animIndex = 1;
		m_modelCom->Set_Animation(m_animIndex, true);
		m_state = EState::IDLE;
	}

	m_colliderCom->Update(XMLoadFloat4x4(m_transformCom->Get_WorldMatrixPtr()));


}

void CBox::Late_Update(_float fTimeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CBox::Render()
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
	
	m_gameInstance.lock()->Add_DebugenderGroup(m_colliderCom);

#endif


	return S_OK;
}

void CBox::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	//if(m_State != STATE::ATTACK &&  self == m_pColliderCom)
	//{
	//m_AnimIndex = 0;
	//
	//m_pModelCom->Set_Animation(m_AnimIndex, false);
	//m_State = ATTACK;
	//
	//}
	//
	//if(m_State == ATTACK)
	//{
	//	if (self == m_Hand_Collider_2 || self == m_Hand_Collider_1)
	//	{
	//		dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner())->Get_Demage();
	//	
	//	}
	//}



}
void CBox::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{


	int i = 0;

	
}

void CBox::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	int i = 0;

}

void CBox::OnGui()
{
}

void CBox::RebindCom()
{

	m_textureCom = Get_Component<CTexture>(L"Com_Texture");
	m_modelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CBox::Bind_ShaderResources()
{
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
		return E_FAIL;

	float fFar = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far", &fFar, sizeof(_float))))
		return E_FAIL;

	float emissive = 0.f;
	if (FAILED(m_shaderCom->Bind_RawValue("g_EmissiveStrength", &emissive, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

HRESULT CBox::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_HalfBoat"), TEXT("Com_Model"), &m_modelCom, nullptr)))
		return E_FAIL;


	CBounding_OBB::BoundingOBBDesc		OBBDesc{};
	OBBDesc.extents = _float3(4.f, 1.5f, 3.f);
	OBBDesc.radians = _float3(0.f, 0.f, 0.f);
	OBBDesc.center = _float3(0.f, 0.f, 0.f);
	OBBDesc.myLayer = ECollisionLayer::ITEM;
	OBBDesc.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_colliderCom, &OBBDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_colliderCom);



	CInventory::InvenDesc storage_desc = {};
	storage_desc.invenType = EInvenType::CHEST;
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Inven"), TEXT("Com_Storage"), &m_storageCom, &storage_desc)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CBox> CBox::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CBox> pInstance(new CBox(pDevice, pContext), [](CBox* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBox");
	}
	return pInstance;
}


shared_ptr<CGameObject> CBox::Clone(void* pArg)
{
	shared_ptr<CBox> pInstance(new CBox(*this), [](CBox* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBox");
	}
	return pInstance;
}

void CBox::Free()
{
	__super::Free();
}
