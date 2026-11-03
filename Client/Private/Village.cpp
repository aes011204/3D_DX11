#include "Village.h"
#include "GameInstance.h"
#include "Model.h"
#include "Bounding_AABB.h"
#include "Inventory.h"
#include "Client_Enum.h"
#include "EventBus.h"

CVillage::CVillage(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject{ device ,context }
{
}

CVillage::CVillage(const CVillage& prototype)
	: CGameObject{ prototype }
{
}

HRESULT CVillage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVillage::Initialize(void* arg)
{


	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	//"Position": [
	//	-3.299999952316284,
	//	2.299999952316284,
	//	0.0
	//] ,
	//	"Rotation" : [
	//		-178.99998474121094,
	//		-1.3660375770996325e-05,
	//		180.0
	//	] ,
	//	"Scale" : [
	//		3.5,
	//		3.5,
	//		3.5
	//	] ,

	m_transformCom->Set_Position(XMVectorSet(-3.3f, 2.2f, 0.f, 1.f));
	m_transformCom->Set_RotationDegree(_float3{ -180.f, -1.3f, 180.f });
	m_transformCom->Set_Scale(_float3{ 3.5f, 3.5f, 3.5f });





	for (_uint i = 0; i < ETOI(EShopTab::END); i++)
	{
		Evt_InvenStorageInitData e = {};
		e.invenPtr = m_shopCom[i];
		e.invenType = m_shopCom[i]->Get_Inventype();
		e.shopTabType = static_cast<EShopTab>(i);
		m_gameInstance.lock()->Get_EventBus()->Publish<Evt_InvenStorageInitData>(e);
	}

	return S_OK;
}

void CVillage::Priority_Update(_float timeDelta)
{
}

void CVillage::Update(_float timeDelta)
{
	if (m_isDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_isDirtyCom = false;
	}
	int a = 1;

	

	m_colliderCom->Update(XMLoadFloat4x4(m_transformCom->Get_WorldMatrixPtr()));
}

void CVillage::Late_Update(_float timeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CVillage::Render()
{




	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMesh = m_modelCom->Get_NumMeshes();

	for (size_t i = 0; i < numMesh; i++)
	{
		m_modelCom->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
		m_modelCom->Bind_Material(m_shaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);
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

void CVillage::OnGui()
{



}

void CVillage::RebindCom()
{
	
	m_textureCom = Get_Component<CTexture>(L"Com_Texture");
	m_modelCom = Get_Component<CModel>(L"Com_Model");
	//m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	//m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
}

HRESULT CVillage::Bind_ShaderResources()
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

	////
	float farZ = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far", &farZ, sizeof(_float))))
		return E_FAIL;
	bool isNight = m_gameInstance.lock()->Get_IsNight();
	_float emissivePower = isNight ? 1.f : 0.f;
	if (FAILED(m_shaderCom->Bind_RawValue("g_EmissiveStrength", &emissivePower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}



HRESULT CVillage::Ready_Components()
{
	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Model_Town"), TEXT("Com_Model"), &m_modelCom, nullptr)))
		return E_FAIL;
	//if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
	//	return E_FAIL;

	CBounding_AABB::BoundingAABBDesc		AABBDesc{};
	AABBDesc.extents = _float3(0.4f, 0.4f, 0.4f);
	AABBDesc.center = _float3(2.21f, -0.66f, 0.5f);
	AABBDesc.myLayer = ECollisionLayer::TRIGGER;
	AABBDesc.otherMask = ECollisionLayer::PLAYER;
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &m_colliderCom, &AABBDesc)))
		return E_FAIL;
	m_gameInstance.lock()->Add_Collider(m_colliderCom);


	for(_uint i =0; i < ETOI(EShopTab::END) ; i++)
	{
		CInventory::InvenDesc storageDesc = {};
		storageDesc.invenType = EInvenType::SHOP;
		storageDesc.equipType = static_cast<EEquipType>(i);
		if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Inven"), TEXT("Com_Storage"), &m_shopCom[i], &storageDesc)))
			return E_FAIL;

	}




	return S_OK;
}

shared_ptr<CVillage> CVillage::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CVillage> instance(new CVillage(device, context), [](CVillage* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVillage");
	}
	return instance;
}


shared_ptr<CGameObject> CVillage::Clone(void* arg)
{
	shared_ptr<CVillage> instance(new CVillage(*this), [](CVillage* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CVillage");
	}
	return instance;
}

void CVillage::Free()
{
	__super::Free();
}
