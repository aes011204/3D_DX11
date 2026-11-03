#include "Sky.h"
#include "GameInstance.h"
#include "Sky_Controller.h"

CSky::CSky(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky& Prototype)
	: CGameObject{Prototype}
{
}

HRESULT CSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CSky::Priority_Update(_float fTimeDelta)
{
}

void CSky::Update(_float fTimeDelta)
{

	m_Acc += fTimeDelta;

}

void CSky::Late_Update(_float fTimeDelta)
{
	m_transformCom->Set_Position(
		XMLoadFloat4(m_gameInstance.lock()->Get_CamPositon()));
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::PRIORITY,static_pointer_cast<CEntity>( shared_from_this()));
}

HRESULT CSky::Render()
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
HRESULT CSky::Ready_Components()
{
	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), &m_VIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), &m_textureCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Noise"), TEXT("Com_TextureNoise"), &m_pTextureNoiseCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Distortion"), TEXT("Com_TextureDistortion"), &m_pTextureDistiortionCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
{
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_textureCom->Bind_ShaderResourceView(m_shaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureNoiseCom->Bind_ShaderResourceView(m_shaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureDistiortionCom->Bind_ShaderResourceView(m_shaderCom, "g_DistortaionTexture", 0)))
		return E_FAIL;

	if (FAILED(m_shaderCom->Bind_RawValue( "g_Acc", &m_Acc, sizeof(float))))
		return E_FAIL;

	float tod01 = m_gameInstance.lock()->Get_TOD01();

	if (FAILED(m_shaderCom->Bind_RawValue("g_fTOD01", &tod01, sizeof(float))))
		return E_FAIL;

	auto diffuse = CSky_Controller::GetInstance()->Get_Diffuse();
	//
	if (FAILED(m_shaderCom->Bind_RawValue("g_SkyColor", &diffuse, sizeof(_float3))))
		return E_FAIL;

		 auto SkyColor = CSky_Controller::GetInstance()->Get_SkyColor();
	 //
	 if (FAILED(m_shaderCom->Bind_RawValue("g_SkyColorOrigin", &SkyColor, sizeof(_float3))))
		 return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_NightT", &NightT, sizeof(_float2))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_MidNightT", &MidNightT, sizeof(_float2))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_DayT", &DayT, sizeof(_float2))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_SunsetT", &SunsetT, sizeof(_float2))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_AfterSunsetT", &AfterSunsetT, sizeof(_float2))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_NightT2", &NightT2, sizeof(_float2))))
	//	return E_FAIL;
	//// 3. 시간대별 색상 (float3) 바인딩
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vNightColor", &Night, sizeof(_float3))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vMidNightColor", &MidNight, sizeof(_float3))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vDayColor", &Day, sizeof(_float3))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vSunsetColor", &Sunset, sizeof(_float3))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vAfterSunsetColor", &AfterSunset, sizeof(_float3))))
	//	return E_FAIL;
	return S_OK;
}

shared_ptr<CSky> CSky::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CSky> pInstance(new CSky(pDevice, pContext), [](CSky* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : sky");
	}
	return pInstance;
}


shared_ptr<CGameObject> CSky::Clone(void* pArg)
{
	shared_ptr<CSky> pInstance(new CSky(*this), [](CSky* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : sky");
	}
	return pInstance;
}


void CSky::Free()
{
}
