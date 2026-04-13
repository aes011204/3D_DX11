#include "Sky.h"
#include "GameInstance.h"

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

	m_pTransformCom->Set_Position(
		XMLoadFloat4(m_pGameInstance.lock()->Get_CamPositon()));
}

void CSky::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::PRIORITY,static_pointer_cast<CEntity>( shared_from_this()));
}

HRESULT CSky::Render()
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
HRESULT CSky::Ready_Components()
{
	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Noise"), TEXT("Com_TextureNoise"), &m_pTextureNoiseCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Distortion"), TEXT("Com_TextureDistortion"), &m_pTextureDistiortionCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureNoiseCom->Bind_ShaderResourceView(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureDistiortionCom->Bind_ShaderResourceView(m_pShaderCom, "g_DistortaionTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue( "g_Acc", &m_Acc, sizeof(float))))
		return E_FAIL;

	float tod01 = m_pGameInstance.lock()->Get_TOD01();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTOD01", &tod01, sizeof(float))))
		return E_FAIL;
	//

	if (FAILED(m_pShaderCom->Bind_RawValue("g_NightT", &NightT, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MidNightT", &MidNightT, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DayT", &DayT, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SunsetT", &SunsetT, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AfterSunsetT", &AfterSunsetT, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_NightT2", &NightT2, sizeof(_float2))))
		return E_FAIL;
	// 3. 시간대별 색상 (float3) 바인딩
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNightColor", &Night, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMidNightColor", &MidNight, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDayColor", &Day, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSunsetColor", &Sunset, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vAfterSunsetColor", &AfterSunset, sizeof(_float3))))
		return E_FAIL;
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
