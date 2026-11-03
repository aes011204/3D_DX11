#include "Camera.h"

#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Shader.h"


CCamera::CCamera(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject(device, context)
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

HRESULT CCamera::Initialize(void* arg)
{
	if (arg == nullptr)
		return E_FAIL; // 이건 업으면 잘못된거임

	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;



	_uint numViewports = { 1 };
	D3D11_VIEWPORT  viewportDesc = {};
	m_context->RSGetViewports(&numViewports, &viewportDesc);

	CameraDesc* desc = static_cast<CameraDesc*>(arg);

	m_transformCom->Set_State(ETransformState::POSITION, XMLoadFloat4(&desc->eyes));
	m_transformCom->LookAt(XMLoadFloat4(&desc->at));

	m_fovy = desc->fovY;
	m_near = desc->nearZ;
	m_far = desc->farZ;
	m_aspect = viewportDesc.Width / viewportDesc.Height;
	

	Update_TransformMatrices();


	return S_OK;
}

void CCamera::Priority_Update(_float timeDelta)
{
}

void CCamera::Update(_float timeDelta)
{
}

void CCamera::Late_Update(_float timeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}


void CCamera::Update_TransformMatrices()
{

	XMStoreFloat4x4(&m_camTSMatrices[ETOI(ED3DTS::VIEW)],XMMatrixInverse(nullptr, XMLoadFloat4x4(m_transformCom->Get_WorldMatrix())));

	XMStoreFloat4x4(&m_camTSMatrices[ETOI(ED3DTS::PROJ)],XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_near, m_far));
}

shared_ptr<CCamera> CCamera::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CCamera> instance(new CCamera(device, context), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera");
	}
	return instance;
}

shared_ptr<CGameObject> CCamera::Clone(void* arg)
{
	shared_ptr<CCamera> instance(new CCamera(*this), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CCamera");
	}
	return instance;
}

void CCamera::Free()
{
	__super::Free();
}
