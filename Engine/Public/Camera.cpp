#include "Camera.h"

#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Shader.h"


CCamera::CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{

}

CCamera::CCamera(const CCamera& prototype)
	:CGameObject(prototype)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL; // 이건 업으면 잘못된거임

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	_uint iNumViewports = { 1 };
	D3D11_VIEWPORT  ViewportDesc = {};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vEyes));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	m_fFovy = pDesc->fFovY;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;
	m_fAspect = ViewportDesc.Width / ViewportDesc.Height;
	

	Update_TransformMatrices();


	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}


void CCamera::Update_TransformMatrices()
{

	m_pGameInstance.lock()->Set_Transform(D3DTS::VIEW,
		XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix())));

	m_pGameInstance.lock()->Set_Transform(D3DTS::PROJ,
		XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

shared_ptr<CCamera> CCamera::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CCamera> pInstance(new CCamera(pDevice, pContext), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera");
	}
	return pInstance;
}

shared_ptr<CGameObject> CCamera::Clone(void* pArg)
{
	shared_ptr<CCamera> pInstance(new CCamera(*this), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera");
	}
	return pInstance;
}

void CCamera::Free()
{
	__super::Free();
}
