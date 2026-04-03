#include "Camera_Play.h"
#include "DInput_Manager.h"
#include "Transform.h"
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
	m_pTarget = pDesc->target;

	m_pTargetTransform = dynamic_pointer_cast<CTransform>(m_pTarget.lock()->Get_Component(g_strTransformTag));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_fMouseSensor = 0.2f;


	m_MinPitch = 5.f;
	m_MaxPitch = 80.f;

	m_MinDistance = 8.f;
	m_MaxDistance = 20.f;

	m_fDistance = 20.f;
	return S_OK;
}

void CCamera_Play::Priority_Update(_float fTimeDelta)
{
	// 움직임이 없으면 원래 상태로 돌아간다
	CDInput_Manager* dinput = m_pGameInstance.lock()->Get_DInput_Manger();


	_long dx = dinput->Get_DIMouseMove(DIMM::X);
	_long dy = dinput->Get_DIMouseMove(DIMM::Y);
	



	// 타겟을 항상 보고있다
	if (auto pTarget = m_pTarget.lock())
	{
		
		_vector targetLook = m_pTargetTransform.lock()->Get_State(STATE::LOOK);
		_float3 vLook;
		XMStoreFloat3(&vLook, XMVector3Normalize(targetLook));

		
		float targetDegree = XMConvertToDegrees(atan2f(vLook.x, vLook.z));

		
		m_Pitch += dy * m_fMouseSensor;
		m_Yaw += dx * m_fMouseSensor;

		auto NormalizeAngle = [](float angle) {
			while (angle > 180.f) angle -= 360.f;
			while (angle < -180.f) angle += 360.f;
			return angle;
			};

		m_Yaw = NormalizeAngle(m_Yaw);
		targetDegree = NormalizeAngle(targetDegree);
		//targetDegree = m_Yaw;
		
		if (dx == 0 && dy == 0)
		{
			float delta = NormalizeAngle(targetDegree - m_Yaw);
			m_Yaw += delta * 15.f * fTimeDelta; // 추적 속도 (조절 가능)
		}
		m_Yaw = NormalizeAngle(m_Yaw);

		
		m_Pitch = clamp(m_Pitch, -5.f, 90.f);
		_float normalizePitch = (m_Pitch - m_MinPitch) / (m_MaxPitch - m_MinPitch);
		m_fDistance = lerp(m_MinDistance, m_MaxDistance, normalizePitch);

		
		float yawRad = XMConvertToRadians(m_Yaw + 180.f);
		float pitchRad = XMConvertToRadians(m_Pitch);

		float x = m_fDistance * cosf(pitchRad) * sinf(yawRad);
		float y = m_fDistance * sinf(pitchRad);
		float z = m_fDistance * cosf(pitchRad) * cosf(yawRad);

		_vector offset = XMVectorSet(x, y, z, 0.f);

	
		_vector targetPos = m_pTargetTransform.lock()->Get_Position() + XMVectorSet(0.f, 2.0f, 0.f, 0.f);
		_vector camPos = targetPos + offset;

		
		_vector currentPos = m_pTransformCom->Get_Position();
		_vector newPos = XMVectorLerp(currentPos, camPos, 8.f * fTimeDelta);

		m_pTransformCom->Set_Position(newPos);
		m_pTransformCom->LookAt(targetPos);

	}
	__super::Update_TransformMatrices();

}


void CCamera_Play::SetTarget(weak_ptr<CGameObject> target, _float startPitchAngle, _float startYawAngle, _float startDistance , _float fTimeDelta)
{
	
}

void CCamera_Play::Update(_float fTimeDelta)
{
	// 레이충돌로 장애물이 있을경우 앞으로 당기던가 해야함
}

void CCamera_Play::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Play::Render()
{
	return S_OK;
}

void CCamera_Play::Start_Targetting(_float4 startPos, _float degree, _float distance)
{

}

shared_ptr<CCamera_Play> CCamera_Play::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CCamera_Play> pInstance(new CCamera_Play(pDevice, pContext), [](CCamera* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Play");
	}
	return pInstance;
}

shared_ptr<CGameObject> CCamera_Play::Clone(void* pArg)
{
	shared_ptr<CCamera_Play> pInstance(new CCamera_Play(*this), [](CCamera* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Play");
	}
	return pInstance;
}

void CCamera_Play::Free()
{
}
