#include "Light.h"
#include "GameInstance.h"
CLight::CLight()
{
}

CLight::~CLight()
{
	Free();
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    memcpy(&m_LightDesc, &LightDesc, sizeof(LIGHT_DESC));

    return S_OK;
}

HRESULT CLight::Render(shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
	_uint iShaderPass = {};


	if(LIGHT::DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof m_LightDesc.vDirection)))
			return E_FAIL;

		iShaderPass = ETOI(DEFERRED::DIRECTIONAL);
	}
	else if(LIGHT::POINT == m_LightDesc.eType)
	{
		iShaderPass = ETOI(DEFERRED::POINT);
	}


	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof m_LightDesc.vDiffuse)))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof m_LightDesc.vAmbient)))
		return E_FAIL;



	pShader->Begin(iShaderPass);

	return pVIBuffer->Render();
}

shared_ptr<CLight> CLight::Create(const LIGHT_DESC& LightDesc)
{
	shared_ptr<CLight> pInstance(new CLight(), [](CLight* p) {p->Free(); delete(p);});

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		
	}
	return pInstance;
}

void CLight::Free()
{
    __super::Free();
}
