#include "Camera_Play.h"

CCamera_Play::CCamera_Play(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCamera(pDevice, pContext)
{
}

CCamera_Play::CCamera_Play(const CCamera& prototype)
	:CCamera(prototype)
{
}

HRESULT CCamera_Play::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Play::Initialize(void* pArg)
{
	CAMERAPLAY_DESC* pDesc = static_cast<CAMERAPLAY_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    return S_OK;
}

void CCamera_Play::Priority_Update(_float fTimeDelta)
{
	
    __super::Update_TransformMatrices();
}

void CCamera_Play::Update(_float fTimeDelta)
{
}

void CCamera_Play::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Play::Render()
{
    return S_OK;
}

shared_ptr<CCamera_Play> CCamera_Play::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CCamera_Play> pInstance(new CCamera_Play(pDevice, pContext), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Play");
	}
	return pInstance;
}

shared_ptr<CGameObject> CCamera_Play::Clone(void* pArg)
{
	shared_ptr<CCamera_Play> pInstance(new CCamera_Play(*this), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Play");
	}
	return pInstance;
}

void CCamera_Play::Free()
{
}
