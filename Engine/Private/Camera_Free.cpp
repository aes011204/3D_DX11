#include "Camera_Free.h"
#include "DInput_Manager.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CCamera(device, context)
{
}

CCamera_Free::CCamera_Free(const CCamera& prototype)
	:CCamera(prototype)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* arg)
{
	CameraFreeDesc* desc = static_cast<CameraFreeDesc*>(arg);
	m_mouseSensor = desc->mouseSensor;

	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Update(_float timeDelta)
{

	if(m_stop != true)
	{
		CDInput_Manager* dinput = m_gameInstance.lock()->Get_DInput_Manger();

		if (dinput->KeyPress(DIK_UP))
		{
			m_transformCom->Go_Forward(timeDelta);
		}

		if (dinput->KeyPress(DIK_DOWN))
		{
			m_transformCom->Go_Backward(timeDelta);
		}

		if (dinput->KeyPress(DIK_LEFT))
		{
			m_transformCom->Go_Left(timeDelta);
		}

		if (dinput->KeyPress(DIK_RIGHT))
		{
			m_transformCom->Go_Right(timeDelta);
		}


		if (dinput->KeyPress(DIK_E))
		{
			m_transformCom->Go_Up(timeDelta);
		}


		if (dinput->KeyPress(DIK_Q))
		{
			m_transformCom->Go_Down(timeDelta);
		}

		_long		mouseMove = {};

		if (mouseMove = dinput->Get_DIMouseMove(EMouseMove::X))
		{
			m_transformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), timeDelta * mouseMove * m_mouseSensor);
		}

		if (mouseMove = dinput->Get_DIMouseMove(EMouseMove::Y))
		{
			m_transformCom->Turn(m_transformCom->Get_State(ETransformState::RIGHT), timeDelta * mouseMove * m_mouseSensor);
		}
		//long dx = dinput->Get_DIMouseMove(DIMM::X);
		//long dy = dinput->Get_DIMouseMove(DIMM::Y);
		//printf("dx=%ld dy=%ld\n", dx, dy);
	}

	
	__super::Update_TransformMatrices();
}

void CCamera_Free::Update(_float timeDelta)
{
}

void CCamera_Free::Late_Update(_float timeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::OnGui()
{
	CCamera::OnGui();

	const _float4x4& mat = *(m_transformCom->Get_WorldMatrix());

	_float3 euler;

	// Pitch (X축 회전)
	euler.x = asinf(-mat._32);

	// 오차 범위 체크 (Gimbal Lock 방지)
	if (cosf(euler.x) > 0.0001f)
	{
		euler.y = atan2f(mat._31, mat._33); // Yaw
		euler.z = atan2f(mat._12, mat._22); // Roll
	}
	else
	{
		euler.y = 0.0f;
		euler.z = atan2f(-mat._21, mat._11);
	}

	// 라디안을 Degree로 변환
	euler.x = XMConvertToDegrees(euler.x);
	euler.y = XMConvertToDegrees(euler.y);
	euler.z = XMConvertToDegrees(euler.z);



	ImGui::Text("Rotation (Euler): X:%.2f, Y:%.2f, Z:%.2f", euler.x, euler.y, euler.z);
}

shared_ptr<CCamera_Free> CCamera_Free::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CCamera_Free> instance(new CCamera_Free(device, context), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
	}
	return instance;
}

shared_ptr<CGameObject> CCamera_Free::Clone(void* arg)
{
	shared_ptr<CCamera_Free> instance(new CCamera_Free(*this), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
	}
	return instance;
}

void CCamera_Free::Free()
{
}
