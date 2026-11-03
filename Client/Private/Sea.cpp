#include "Sea.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Entity.h"
#include "PipeLine.h"
#include "EventBus.h"
#include "Event_Struct.h"
#include "Terrain.h"
#include "VIBuffer_Sea.h"
#include "Sea_Manager.h"
#include <numbers>

#include "Sky_Controller.h"
#include "PlayerBoat.h"


CSea::CSea(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext), m_seaManager(CSea_Manager::GetInstance())

{
}

CSea::CSea(const CSea& prototype)
	:CGameObject(prototype), m_seaManager(CSea_Manager::GetInstance())

{

}

HRESULT CSea::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSea::Initialize(void* pArg)
{

	m_skyControl = (CSky_Controller::GetInstance());
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	SeaDesc			Desc{};

	// float2 dir1 = float2(1.f, 1.f);
  // float2 dir2 = float2(0.f,-0.46f);
  // float2 dir3 = float2(-1.f, .3f);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir1, .43f, 17.f, 2.5f, g_Time);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir2, 0.21f, 7.4f, 1.8f, g_Time);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir3, 0.08f, 2.7f, 1.f, g_Time);

	



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
	_float3 snapedPos = m_cachedVIBufferCom->Snapping((m_seaManager.lock()->Get_GlobalY()));
	m_transformCom->Set_Position(XMLoadFloat3(&snapedPos));


	//m_fX += 10.f * fTimeDelta;
	//__super::Update_Transform();
	int a = 1;

}

void CSea::Late_Update(_float fTimeDelta)
{
	int a = 1;
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::SEA, static_pointer_cast<CSea>(shared_from_this()));
}




HRESULT CSea::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_shaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_VIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_cachedVIBufferCom->Render(m_shaderCom)))
		return E_FAIL;

	return S_OK;
}

_vector CSea::Get_WorldPos()
{
	{
		return m_transformCom->Get_Position();
	}
}

void CSea::OnGui()
{

}

HRESULT CSea::Ready_Components()
{
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_VIBuffer_Sea"), TEXT("Com_VIBuffer"), &m_VIBufferCom, nullptr)))
		return E_FAIL;
	m_cachedVIBufferCom = dynamic_pointer_cast<CVIBuffer_Sea> (m_VIBufferCom);
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_Sea"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Texture_TerrainHeight"), TEXT("Com_Texture"), &m_textureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSea::Bind_ShaderResources()
{
	auto Sea = m_seaManager.lock();
	_float fTime = Sea->Get_AccTime();
	int WaveCount = Sea->Get_WaveCount();
	//float depthMask01 = Sea->Get_DepthMask01();
	const WaveDesc* waveDesc = Sea->Get_WaveDescArray();

	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
		return E_FAIL;


	if (FAILED(m_shaderCom->Bind_RawValue("g_Time", &fTime, sizeof(_float))))
		return E_FAIL;


	const LightDesc* desc = m_gameInstance.lock()->Get_LightDesc(0);
	if (nullptr == desc)
		return E_FAIL;
	const _float4& dir = m_skyControl->Get_Direction();
	const _float4& diff = m_skyControl->Get_Diffuse();
	const _float4& amb = m_skyControl->Get_Ambient();
	const _float4& spec = m_skyControl->Get_Specular();
	if (FAILED(m_shaderCom->Bind_RawValue("g_vLightDir", &dir, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vLightDiffuse", &diff, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vLightAmbient", &amb, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vLightSpecular", &spec, sizeof(_float4))))
		return E_FAIL;

		_int iPlayerPointLightActive = 0;
	_float4 vPlayerLightPos = _float4(0.f, 0.f, 0.f, 1.f);
	_float fPlayerLightRange = 0.f;
	_float4 vPlayerLightDiffuse = _float4(0.f, 0.f, 0.f, 1.f);
	_float4 vPlayerLightSpecular = _float4(0.f, 0.f, 0.f, 1.f);

	auto pPlayerObject = m_gameInstance.lock()->Get_GameObject(m_gameInstance.lock()->Get_Current_LevelIdx(), L"Layer_Player", 0);
	auto pPlayerBoat = dynamic_pointer_cast<CPlayerBoat>(pPlayerObject);
	auto pPlayerLight = pPlayerBoat ? pPlayerBoat->Get_Light().lock() : nullptr;
	if (pPlayerLight != nullptr && pPlayerLight->Get_Active())
	{
		const LightDesc* pPlayerLightDesc = pPlayerLight->Get_LightDesc();
		if (pPlayerLightDesc != nullptr && pPlayerLightDesc->type == ELightType::POINT)
		{
			iPlayerPointLightActive = 1;
			vPlayerLightPos = pPlayerLightDesc->position;
			fPlayerLightRange = max(pPlayerLightDesc->range, 12.f);
			vPlayerLightDiffuse = pPlayerLightDesc->diffuse;
			vPlayerLightSpecular = pPlayerLightDesc->specular;
		}
	}
if (FAILED(m_shaderCom->Bind_RawValue("g_iPlayerPointLightActive", &iPlayerPointLightActive, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vPlayerLightPos", &vPlayerLightPos, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_fPlayerLightRange", &fPlayerLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vPlayerLightDiffuse", &vPlayerLightDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vPlayerLightSpecular", &vPlayerLightSpecular, sizeof(_float4))))
		return E_FAIL;

	///////Wave///////
	if (FAILED(m_shaderCom->Bind_RawValue("g_WaveCount", &WaveCount, sizeof(int))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_Waves", waveDesc, sizeof(WaveDesc)*10)))
		return E_FAIL; // 고정 배열 10
	

	if (FAILED(m_shaderCom->Bind_RawValue("g_TerrainSize", &Sea->m_terrainSize, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_TerrainPos", &Sea->m_terrainPos, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_deepColor", &Sea->m_deepColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_shallowColor", &Sea->m_shallowColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_textureCom->Bind_ShaderResourceView(m_shaderCom, "g_TerrainHeight", 0)))
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
