#include "Terrain.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Entity.h"
#include "PipeLine.h"
#include "EventBus.h"
#include "Event_Struct.h"


CTerrain::CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& prototype)
	:CGameObject(prototype)

{

}

HRESULT CTerrain::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	TERRAIN_DESC			Desc{};

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	int a = 1;
}

void CTerrain::Update(_float fTimeDelta)
{

	//m_fX += 10.f * fTimeDelta;
	//__super::Update_Transform();
	int a = 1;

}

void CTerrain::Late_Update(_float fTimeDelta)
{
	int a = 1;
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CTerrain>(shared_from_this()));
}

HRESULT CTerrain::Render()
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

void CTerrain::OnGui()
{

}

HRESULT CTerrain::Ready_Components()
{
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),& m_pVIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),& m_pShaderCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),& m_pTextureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
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

	const LIGHT_DESC* desc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == desc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &desc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &desc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &desc->vAmbition, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &desc->vSpecular, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

shared_ptr<CTerrain> CTerrain::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CTerrain> pInstance(new CTerrain(pDevice, pContext), [](CTerrain* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
	}
	return pInstance;
}


shared_ptr<CGameObject> CTerrain::Clone(void* pArg)
{
	shared_ptr<CTerrain> pInstance(new CTerrain(*this), [](CTerrain* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();
}
