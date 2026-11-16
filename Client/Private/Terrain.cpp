#include "Terrain.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Entity.h"
#include "PipeLine.h"
#include "EventBus.h"
#include "Event_Struct.h"
#include "Sea_Manager.h"
#include "VIBuffer_Terrain.h"


CTerrain::CTerrain(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CGameObject(device, context)
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

HRESULT CTerrain::Initialize(void* arg)
{
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	TerrainDesc			desc{};

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	_float3 pos = { -750.f, -100.f, -750.f };

	m_transformCom->Set_Position(XMLoadFloat3(&pos));


	CSea_Manager::GetInstance()->Set_TerrainBuffer(dynamic_pointer_cast<Engine::CVIBuffer_Terrain>(m_VIBufferCom));
	return S_OK;
}

void CTerrain::Priority_Update(_float timeDelta)
{
	int a = 1;
}

void CTerrain::Update(_float timeDelta)
{

	//m_fX += 10.f * fTimeDelta;
	//__super::Update_Transform();
	int a = 1;

}

void CTerrain::Late_Update(_float timeDelta)
{
	int a = 1;
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CTerrain>(shared_from_this()));
}

HRESULT CTerrain::Render()
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

void CTerrain::OnGui()
{

}

HRESULT CTerrain::Ready_Components()
{
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),& m_VIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),& m_shaderCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),& m_textureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
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

	//const LIGHT_DESC* desc = m_pGameInstance.lock()->Get_LightDesc(0);
	//if (nullptr == desc)
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &desc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &desc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &desc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &desc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//////////////////
	_float4 sandColor  = _float4(0.482f, 0.482f, 0.418f, 1.f);
	_float4 algaeColor = _float4(0.422f, 0.433f, 0.321f, 1.f);
	_float4 rockColor = _float4(0.283f, 0.283f, 0.283f, 1.f);
	//_float fTiling = 80.f; 

	m_shaderCom->Bind_RawValue("g_vSandColor", &sandColor, sizeof(_float4));
	m_shaderCom->Bind_RawValue("g_vAlgaeColor", &algaeColor, sizeof(_float4));
	m_shaderCom->Bind_RawValue("g_vRockColor", &rockColor, sizeof(_float4));
	//m_pShaderCom->Bind_RawValue("g_fTiling", &fTiling, sizeof(_float));

	float farZ = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far", &farZ, sizeof(_float))))

	return S_OK;
}

shared_ptr<CTerrain> CTerrain::Create(ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CTerrain> instance(new CTerrain(device, context), [](CTerrain* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
	}
	return instance;
}


shared_ptr<CGameObject> CTerrain::Clone(void* arg)
{
	shared_ptr<CTerrain> instance(new CTerrain(*this), [](CTerrain* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
	}
	return instance;
}

void CTerrain::Free()
{
	__super::Free();
}
