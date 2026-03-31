#include "Sea.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Entity.h"
#include "PipeLine.h"
#include "EventBus.h"
#include "Event_Struct.h"
#include "Terrain.h"
#include "VIBuffer_Sea.h"



CSea::CSea(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

CSea::CSea(const CSea& prototype)
	:CGameObject(prototype)

{

}

HRESULT CSea::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSea::Initialize(void* pArg)
{
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	SEA_DESC			Desc{};

	// float2 dir1 = float2(1.f, 1.f);
  // float2 dir2 = float2(0.f,-0.46f);
  // float2 dir3 = float2(-1.f, .3f);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir1, .43f, 17.f, 2.5f, g_Time);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir2, 0.21f, 7.4f, 1.8f, g_Time);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir3, 0.08f, 2.7f, 1.f, g_Time);


	m_WaveCount = 3;

	m_waveDesc[0].dir = _float2(1.f, 1.f);
	m_waveDesc[0].waveHeight = .43f;
	m_waveDesc[0].waveLength = 17.f;
	m_waveDesc[0].speed = 2.5f;

	m_waveDesc[1].dir = _float2(0.f, -0.46f);
	m_waveDesc[1].waveHeight = 0.21f;
	m_waveDesc[1].waveLength = 7.4f;
	m_waveDesc[1].speed = 1.8f;

	m_waveDesc[2].dir = _float2(-1.f, .3f);
	m_waveDesc[2].waveHeight = 0.08f;
	m_waveDesc[2].waveLength = 2.7f;
	m_waveDesc[2].speed = 1.f;




	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CSea::Priority_Update(_float fTimeDelta)
{
	int a = 1;
}

void CSea::Update(_float fTimeDelta)
{
	// 스냅핑
	_float3 snapedPos = m_pVIBufferComCashing->Snaping(XMVectorGetY(m_pTransformCom->Get_Position()));
	m_pTransformCom->Set_Position(XMLoadFloat3(&snapedPos));


	//m_fX += 10.f * fTimeDelta;
	//__super::Update_Transform();
	int a = 1;

	m_AccTime += fTimeDelta;

}

void CSea::Late_Update(_float fTimeDelta)
{
	int a = 1;
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CSea>(shared_from_this()));
}

HRESULT CSea::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferComCashing->Render(m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CSea::OnGui()
{

}

HRESULT CSea::Ready_Components()
{
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Sea"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
		return E_FAIL;
	m_pVIBufferComCashing = dynamic_pointer_cast<CVIBuffer_Sea> (m_pVIBufferCom);
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_Sea"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Black"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSea::Bind_ShaderResources()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_AccTime, sizeof(_float))))
		return E_FAIL;


	const LIGHT_DESC* desc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == desc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &desc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &desc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &desc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &desc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	///////Wave///////
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WaveCount", &m_WaveCount, sizeof(int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Waves", &m_waveDesc, sizeof(Wave_Desc)*10)))
		return E_FAIL; // 고정 배열 10
	if (FAILED(m_pShaderCom->Bind_RawValue("g_depthMask01", &m_depthMask01, sizeof(int))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CSea> CSea::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CSea> pInstance(new CSea(pDevice, pContext), [](CSea* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSea");
	}
	return pInstance;
}


shared_ptr<CGameObject> CSea::Clone(void* pArg)
{
	shared_ptr<CSea> pInstance(new CSea(*this), [](CSea* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSea");
	}
	return pInstance;
}

void CSea::Free()
{
	__super::Free();
}
