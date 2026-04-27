#include "Camera_Free.h"
#include "DInput_Manager.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CCamera(pDevice, pContext)
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

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CAMERAFREE_DESC* pDesc = static_cast<CAMERAFREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{

	if(m_stop != true)
	{
		CDInput_Manager* dinput = m_pGameInstance.lock()->Get_DInput_Manger();

		if (dinput->KeyPress(DIK_UP))
		{
			m_pTransformCom->Go_Forward(fTimeDelta);
		}

		if (dinput->KeyPress(DIK_DOWN))
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}

		if (dinput->KeyPress(DIK_LEFT))
		{
			m_pTransformCom->Go_Left(fTimeDelta);
		}

		if (dinput->KeyPress(DIK_RIGHT))
		{
			m_pTransformCom->Go_Right(fTimeDelta);
		}


		if (dinput->KeyPress(DIK_E))
		{
			m_pTransformCom->Go_Up(fTimeDelta);
		}


		if (dinput->KeyPress(DIK_Q))
		{
			m_pTransformCom->Go_Down(fTimeDelta);
		}

		_long		MouseMove = {};

		if (MouseMove = dinput->Get_DIMouseMove(DIMM::X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}

		if (MouseMove = dinput->Get_DIMouseMove(DIMM::Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
		//long dx = dinput->Get_DIMouseMove(DIMM::X);
		//long dy = dinput->Get_DIMouseMove(DIMM::Y);
		//printf("dx=%ld dy=%ld\n", dx, dy);
	}

	
	__super::Update_TransformMatrices();
}

void CCamera_Free::Update(_float fTimeDelta)
{
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::OnGui()
{
	CCamera::OnGui();

	const _float4x4& mat = *(m_pTransformCom->Get_WorldMatrix());

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

shared_ptr<CCamera_Free> CCamera_Free::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CCamera_Free> pInstance(new CCamera_Free(pDevice, pContext), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
	}
	return pInstance;
}

shared_ptr<CGameObject> CCamera_Free::Clone(void* pArg)
{
	shared_ptr<CCamera_Free> pInstance(new CCamera_Free(*this), [](CCamera* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
	}
	return pInstance;
}

void CCamera_Free::Free()
{
}
