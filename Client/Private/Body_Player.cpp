#include "Body_Player.h"
#include "Model.h"




CBody_Player::CBody_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CPartObject(pDevice, pContext)
{
}

CBody_Player::CBody_Player(const CBody_Player& prototype)
	: CPartObject(prototype)
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	auto pDesc = static_cast<BodyPlayer_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pModelCom->Set_Animation(0,true)

	return CPartObject::Initialize(pArg);
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{
}

void CBody_Player::Update(_float fTimeDelta)
{
}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	
	m_pTransformCom->Update_WorldMatrix();
  	Update_CombinedWorldMatirix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix()));
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));

}

HRESULT CBody_Player::Render()
{
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

}

void CBody_Player::OnGui()
{
	
}

void CBody_Player::RebindCom()
{
	
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");
	
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
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

const _float4x4* CBody_Player::Get_SocketBoneMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBody_Player::Ready_Components()
{
	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_PlayerBoat"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;
	//if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
	//	return E_FAIL;

	return S_OK;
}

shared_ptr<CBody_Player> CBody_Player::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CBody_Player> pInstance(new CBody_Player(pDevice, pContext), [](CBody_Player* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
	}
	return pInstance;
}


shared_ptr<CGameObject> CBody_Player::Clone(void* pArg)
{
	shared_ptr<CBody_Player> pInstance(new CBody_Player(*this), [](CBody_Player* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
	}
	return pInstance;
}

void CBody_Player::Free()
{
	CPartObject::Free();
}
