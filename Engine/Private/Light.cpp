#include "Light.h"
#include "GameInstance.h"
CLight::CLight()
{
}

CLight::~CLight()
{
	Free();
}

HRESULT CLight::Initialize(const LightDesc& lightDesc)
{
    memcpy(&m_lightDesc, &lightDesc, sizeof(LightDesc));

    return S_OK;
}

HRESULT CLight::Render(shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
	_uint iShaderPass = {};


	if(ELightType::DIRECTIONAL == m_lightDesc.type)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_lightDesc.direction, sizeof m_lightDesc.direction)))
			return E_FAIL;

		iShaderPass = ETOI(EDeferred::DIRECTIONAL);
	}
	else if(ELightType::POINT == m_lightDesc.type)
	{

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_lightDesc.position, sizeof m_lightDesc.position)))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_lightDesc.range, sizeof m_lightDesc.range)))
			return E_FAIL;
		iShaderPass = ETOI(EDeferred::POINT);
	}


	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_lightDesc.diffuse, sizeof m_lightDesc.diffuse)))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_lightDesc.ambient, sizeof m_lightDesc.ambient)))
		return E_FAIL;
	//if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof m_LightDesc.vSpecular)))
	//	return E_FAIL;


	pShader->Begin(iShaderPass);

	return pVIBuffer->Render();
}

shared_ptr<CLight> CLight::Create(const LightDesc& lightDesc)
{
	shared_ptr<CLight> pInstance(new CLight(), [](CLight* p) {p->Free(); delete(p);});

	if (FAILED(pInstance->Initialize(lightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		
	}
	return pInstance;
}

void CLight::Free()
{
    __super::Free();
}
