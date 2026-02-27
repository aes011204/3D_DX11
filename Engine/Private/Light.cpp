#include "Light.h"

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
