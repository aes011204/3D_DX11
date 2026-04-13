#include "Village.h"
#include "GameInstance.h"
#include "Model.h"
#include "Bounding_AABB.h"

CVillage::CVillage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
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

HRESULT CVillage::Initialize(void* pArg)
{


	if (FAILED(__super::Initialize(pArg)))
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

	m_pTransformCom->Set_Position(XMVectorSet(-3.3f, 2.2f, 0.f, 1.f));
	m_pTransformCom->Set_RotationDegree(_float3{ -180.f, -1.3f, 180.f });
	m_pTransformCom->Set_Scale(_float3{ 3.5f, 3.5f, 3.5f });

	return S_OK;
}

void CVillage::Priority_Update(_float fTimeDelta)
{
}

void CVillage::Update(_float fTimeDelta)
{
	if (m_bIsDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_bIsDirtyCom = false;
	}
	int a = 1;

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CVillage::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CVillage::Render()
{
#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_pColliderCom->Render();


#endif




	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMesh; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}



	return S_OK;
}

void CVillage::OnGui()
{



}

void CVillage::RebindCom()
{
	
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");
	//m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	//m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
}

HRESULT CVillage::Bind_ShaderResources()
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

	////
	
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
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Town"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;
	//if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
	//	return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		AABBDesc{};
	AABBDesc.vExtents = _float3(0.4f, 0.4f, 0.4f);
	AABBDesc.vCenter = _float3(2.21f, -0.66f, 0.5f);
	AABBDesc.MyLayer = COLLISION_LAYER::TRIGGER;
	AABBDesc.OtherMask = COLLISION_LAYER::PLAYER;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), &m_pColliderCom, &AABBDesc)))
		return E_FAIL;
	m_pGameInstance.lock()->Add_Collider(m_pColliderCom);


	return S_OK;
}

shared_ptr<CVillage> CVillage::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CVillage> pInstance(new CVillage(pDevice, pContext), [](CVillage* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVillage");
	}
	return pInstance;
}


shared_ptr<CGameObject> CVillage::Clone(void* pArg)
{
	shared_ptr<CVillage> pInstance(new CVillage(*this), [](CVillage* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVillage");
	}
	return pInstance;
}

void CVillage::Free()
{
	__super::Free();
}
