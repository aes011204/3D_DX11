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


CSea::CSea(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CGameObject(device, context), m_seaManager(CSea_Manager::GetInstance())

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

HRESULT CSea::Initialize(void* arg)
{

	m_skyControl = (CSky_Controller::GetInstance());
	/* 백그라운드의 멤버를 채워넣어야한다면 여기서 채운다. */
	SeaDesc			desc{};

	// float2 dir1 = float2(1.f, 1.f);
  // float2 dir2 = float2(0.f,-0.46f);
  // float2 dir3 = float2(-1.f, .3f);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir1, .43f, 17.f, 2.5f, g_Time);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir2, 0.21f, 7.4f, 1.8f, g_Time);
   //offset += Calculate_GerstnerWave_Overlap(pos, Dir3, 0.08f, 2.7f, 1.f, g_Time);

	



	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CSea::Priority_Update(_float timeDelta)
{
	int a = 1;
}

void CSea::Update(_float timeDelta)
{
	// 스냅핑
	_float3 snapedPos = m_cachedVIBufferCom->Snapping((m_seaManager.lock()->Get_GlobalY()));
	m_transformCom->Set_Position(XMLoadFloat3(&snapedPos));


	//m_fX += 10.f * fTimeDelta;
	//__super::Update_Transform();
	int a = 1;

}

void CSea::Late_Update(_float timeDelta)
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
	auto sea = m_seaManager.lock();
	_float time = sea->Get_AccTime();
	int waveCount = sea->Get_WaveCount();
	//float depthMask01 = Sea->Get_DepthMask01();
	const WaveDesc* waveDesc = sea->Get_WaveDescArray();

	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
		return E_FAIL;


	if (FAILED(m_shaderCom->Bind_RawValue("g_Time", &time, sizeof(_float))))
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

		_int playerPointLightActive = 0;
	_float4 playerLightPos = _float4(0.f, 0.f, 0.f, 1.f);
	_float playerLightRange = 0.f;
	_float4 playerLightDiffuse = _float4(0.f, 0.f, 0.f, 1.f);
	_float4 playerLightSpecular = _float4(0.f, 0.f, 0.f, 1.f);

	auto playerObject = m_gameInstance.lock()->Get_GameObject(m_gameInstance.lock()->Get_Current_LevelIdx(), L"Layer_Player", 0);
	auto playerBoat = dynamic_pointer_cast<CPlayerBoat>(playerObject);
	auto playerLight = playerBoat ? playerBoat->Get_Light().lock() : nullptr;
	if (playerLight != nullptr && playerLight->Get_Active())
	{
		const LightDesc* playerLightDesc = playerLight->Get_LightDesc();
		if (playerLightDesc != nullptr && playerLightDesc->type == ELightType::POINT)
		{
			playerPointLightActive = 1;
			playerLightPos = playerLightDesc->position;
			playerLightRange = max(playerLightDesc->range, 12.f);
			playerLightDiffuse = playerLightDesc->diffuse;
			playerLightSpecular = playerLightDesc->specular;
		}
	}
if (FAILED(m_shaderCom->Bind_RawValue("g_iPlayerPointLightActive", &playerPointLightActive, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vPlayerLightPos", &playerLightPos, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_fPlayerLightRange", &playerLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vPlayerLightDiffuse", &playerLightDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_vPlayerLightSpecular", &playerLightSpecular, sizeof(_float4))))
		return E_FAIL;

	///////Wave///////
	if (FAILED(m_shaderCom->Bind_RawValue("g_WaveCount", &waveCount, sizeof(int))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_Waves", waveDesc, sizeof(WaveDesc)*10)))
		return E_FAIL; // 고정 배열 10
	

	if (FAILED(m_shaderCom->Bind_RawValue("g_TerrainSize", &sea->m_terrainSize, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_TerrainPos", &sea->m_terrainPos, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_deepColor", &sea->m_deepColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_shallowColor", &sea->m_shallowColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_textureCom->Bind_ShaderResourceView(m_shaderCom, "g_TerrainHeight", 0)))
		return E_FAIL;
	return S_OK;
}

shared_ptr<CSea> CSea::Create(ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CSea> instance(new CSea(device, context), [](CSea* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSea");
	}
	return instance;
}


shared_ptr<CGameObject> CSea::Clone(void* arg)
{
	shared_ptr<CSea> instance(new CSea(*this), [](CSea* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CSea");
	}
	return instance;
}

void CSea::Free()
{
	__super::Free();
}
