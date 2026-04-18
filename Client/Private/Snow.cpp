#include "Snow.h"
#include "GameInstance.h"
#include "Model.h"
#include "VIBuffer_Particle_Rect.h"
#include "Texture.h"


CSnow::CSnow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CSnow::CSnow(const CSnow& prototype)
	: CGameObject{ prototype }
{
}

HRESULT CSnow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSnow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Position( XMVectorSet(129.f * 0.5f, 30.f, 129.f * 0.5f, 1.f));

	//m_pSocketMatrix_AttackPoint = m_pModelCom->Get_BoneMatrixPtr(/*"l_botclaw_jnt"*/);

	return S_OK;
}

void CSnow::Priority_Update(_float fTimeDelta)
{
}

void CSnow::Update(_float fTimeDelta)
{
	m_pVIBufferCom->Drop(fTimeDelta);
}

void CSnow::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONLIGHT, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CSnow::Render()
{


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		if (FAILED(m_pVIBufferCom->Bind_Resources()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;





	return S_OK;
}

void CSnow::OnGui()
{
}

void CSnow::RebindCom()
{

	//m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	//m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CSnow::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
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

	return S_OK;
}

HRESULT CSnow::Ready_Components()
{


	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticleRect"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;

		if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Snow"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
			return E_FAIL;
	
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Rect_Snow"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
		return E_FAIL;
	



	return S_OK;
}

shared_ptr<CSnow> CSnow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CSnow> pInstance(new CSnow(pDevice, pContext), [](CSnow* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSnow");
	}
	return pInstance;
}


shared_ptr<CGameObject> CSnow::Clone(void* pArg)
{
	shared_ptr<CSnow> pInstance(new CSnow(*this), [](CSnow* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSnow");
	}
	return pInstance;
}

void CSnow::Free()
{
	__super::Free();
}
