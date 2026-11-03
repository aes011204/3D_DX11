#include "Explosion.h"
#include "GameInstance.h"
#include "Model.h"
#include "VIBuffer_Particle_Point.h"
#include "Texture.h"


CExplosion::CExplosion(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CExplosion::CExplosion(const CExplosion& prototype)
	: CGameObject{ prototype }
{
}

HRESULT CExplosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExplosion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_transformCom->Set_Position( XMVectorSet(50.f, 5.f, 1.f, 1.f));

	//m_pSocketMatrix_AttackPoint = m_pModelCom->Get_BoneMatrixPtr(/*"l_botclaw_jnt"*/);

	return S_OK;
}

void CExplosion::Priority_Update(_float fTimeDelta)
{
}

void CExplosion::Update(_float fTimeDelta)
{
	m_VIBufferCom->Spread(fTimeDelta);
}

void CExplosion::Late_Update(_float fTimeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONLIGHT, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CExplosion::Render()
{


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	
		if (FAILED(m_shaderCom->Begin(0)))
			return E_FAIL;
	
	
		if (FAILED(m_VIBufferCom->Bind_Resources()))
			return E_FAIL;
	
		if (FAILED(m_VIBufferCom->Render()))
			return E_FAIL;
	




	return S_OK;
}

void CExplosion::OnGui()
{
}

void CExplosion::RebindCom()
{

	//m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	//m_pModelCom = Get_Component<CModel>(L"Com_Model");

}

HRESULT CExplosion::Bind_ShaderResources()
{
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_textureCom->Bind_ShaderResourceView(m_shaderCom, "g_DiffuseTexture", 0)))
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

	return S_OK;
}

HRESULT CExplosion::Ready_Components()
{


	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxParticlePoint"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;

		if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Texture_Snow"), TEXT("Com_Texture"), &m_textureCom, nullptr)))
			return E_FAIL;
	
	if (FAILED(Add_Component(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Particle_Point_Explosion"), TEXT("Com_VIBuffer"), &m_VIBufferCom, nullptr)))
		return E_FAIL;
	



	return S_OK;
}

shared_ptr<CExplosion> CExplosion::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CExplosion> pInstance(new CExplosion(pDevice, pContext), [](CExplosion* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CExplosion");
	}
	return pInstance;
}


shared_ptr<CGameObject> CExplosion::Clone(void* pArg)
{
	shared_ptr<CExplosion> pInstance(new CExplosion(*this), [](CExplosion* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CExplosion");
	}
	return pInstance;
}

void CExplosion::Free()
{
	__super::Free();
}
