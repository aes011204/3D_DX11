#include "Camera.h"
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
	m_context->RSGetViewports(&iNumViewports, &ViewportDesc);

	CameraDesc* pDesc = static_cast<CameraDesc*>(pArg);

	m_transformCom->Set_State(ETransformState::POSITION, XMLoadFloat4(&pDesc->eyes));
	m_transformCom->LookAt(XMLoadFloat4(&pDesc->at));

	m_fovy = pDesc->fovY;
	m_near = pDesc->nearZ;
	m_far = pDesc->farZ;
	m_aspect = ViewportDesc.Width / ViewportDesc.Height;


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

	XMStoreFloat4x4(&m_camTSMatrices[ETOI(ED3DTS::VIEW)], XMMatrixInverse(nullptr, XMLoadFloat4x4(m_transformCom->Get_WorldMatrix())));

	XMStoreFloat4x4(&m_camTSMatrices[ETOI(ED3DTS::PROJ)], XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_near, m_far));
}

shared_ptr<CCamera> CCamera::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CCamera> pInstance(new CCamera(pDevice, pContext), [](CCamera* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera");
	}
	return pInstance;
}

shared_ptr<CGameObject> CCamera::Clone(void* pArg)
{
	shared_ptr<CCamera> pInstance(new CCamera(*this), [](CCamera* p) {p->Free(); delete p; });

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
