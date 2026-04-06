#include "Monster_Anim.h"
#include "GameInstance.h"
#include "Model.h"
#include "Collider.h"
//#include "Assimp_Model.h"

CMonster_Anim::CMonster_Anim(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CMonster_Anim::CMonster_Anim(const CMonster_Anim& prototype)
	: CGameObject{prototype}
{
}

HRESULT CMonster_Anim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Anim::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, true);

	return S_OK;
}

void CMonster_Anim::Priority_Update(_float fTimeDelta)
{
}

void CMonster_Anim::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);


m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CMonster_Anim::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CMonster_Anim::Render()
{

#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_IsDebug() == false)
		return S_OK;
	m_pColliderCom->Render();

#endif



	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for(size_t i=0; i< iNumMesh; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i,TextureType_DIFFUSE, 0);
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(i)))
		return E_FAIL;
	}




	return S_OK;
}

void CMonster_Anim::OnGui()
{
}

void CMonster_Anim::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");
	//m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	//m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
}

HRESULT CMonster_Anim::Bind_ShaderResources()
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

HRESULT CMonster_Anim::Ready_Components()
{
	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona_Anim"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};
	OBBDesc.vExtents = _float3(1.f, 1.f, 1.f);
	OBBDesc.vRadians = _float3(0.f, 0.f, 0.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), &m_pColliderCom, &OBBDesc)))
		return E_FAIL;
	return S_OK;
}

shared_ptr<CMonster_Anim> CMonster_Anim::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CMonster_Anim> pInstance(new CMonster_Anim(pDevice, pContext), [](CMonster_Anim* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Anim");
	}
	return pInstance;
}


shared_ptr<CGameObject> CMonster_Anim::Clone(void* pArg)
{
	shared_ptr<CMonster_Anim> pInstance(new CMonster_Anim(*this), [](CMonster_Anim* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Anim");
	}
	return pInstance;
}

void CMonster_Anim::Free()
{
	__super::Free();
}
