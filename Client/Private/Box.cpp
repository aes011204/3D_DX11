#include "Box.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
#include "PlayerBoat.h"
#include "EventBus.h"
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


	m_AnimIndex = 1;
	m_pModelCom->Set_Animation(m_AnimIndex, true);
	m_State = STATE::IDLE;


	{
		Evt_InvenStrageInit_Data e = {};
		e.Inven_ptr = m_pStorageCom;
		e.inventype = m_pStorageCom->Get_Inventype();
		m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_InvenStrageInit_Data>(e);
	}
	_float3 pos = _float3(49.5f, -0.4f, -19.f);
	m_pTransformCom->Set_Position(XMLoadFloat3(&pos));
	
	return S_OK;
}

void CBox::Priority_Update(_float fTimeDelta)
{
}

void CBox::Update(_float fTimeDelta)
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


}

void CBox::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CBox::Render()
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
	
	m_pGameInstance.lock()->Add_DebugenderGroup(m_pColliderCom);

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

	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
		return E_FAIL;

	float fFar = m_pGameInstance.lock()->Get_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Far", &fFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBox::Ready_Components()
{

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_HalfBoat"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;


	CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};
	OBBDesc.vExtents = _float3(4.f, 1.5f, 3.f);
	OBBDesc.vRadians = _float3(0.f, 0.f, 0.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	OBBDesc.MyLayer = COLLISION_LAYER::ITEM;
	OBBDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_pColliderCom, &OBBDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_pColliderCom);



	CInventory::INVEN_DESC storage_desc = {};
	storage_desc.invenType = INVENTYPE::CHEST;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Inven"), TEXT("Com_Storage"), &m_pStorageCom, &storage_desc)))
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
