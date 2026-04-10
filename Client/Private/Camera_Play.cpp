#include "Camera_Play.h"
#include "DInput_Manager.h"
#include "Transform.h"
#include "../../Engine/Public/EventBus.h"

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

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTargetTransform = dynamic_pointer_cast<CTransform>(m_pTarget.lock()->Get_Component(g_strTransformTag));

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeCam>(
		[this](const Evt_ChangeCam& e)
		{
			m_CamCommands = e.commands;
			//m_bChange = true;
			m_FirstFlag = false;
			m_bFinish = false;
		}
	);


	m_fMouseSensor = 0.2f;


	m_MinPitch = 5.f;
	m_MaxPitch = 80.f;

	m_MinDistance = 7.f;
	m_MaxDistance = 20.f;

	m_fDistance = 18.f;
	return S_OK;
}

void CCamera_Play::Priority_Update(_float fTimeDelta)
{
	if (m_CamCommands.empty())
		return;

	/*if (m_bChange == true)
	{
		m_CamMode = m_ReservCamMode;
		m_NextCamMode = m_ReservNextCamMode;
		m_bChange = false;
	}*/
	auto pCurrentDesc = m_CamCommands.front();
	CAM_MODE eMode = pCurrentDesc->eMode;
	switch (eMode)
	{
	case CAM_MODE::FOLLOW:
		Update_Follow(fTimeDelta, pCurrentDesc);

		break;
	case CAM_MODE::STOP:
		__super::Update_TransformMatrices();
		break;
	case CAM_MODE::LERP:
		if (false == Update_Lerp(fTimeDelta, pCurrentDesc))
			m_bFinish = true;

		break;

	case CAM_MODE::END:
		break;
	}

	if (m_bFinish == true)
	{
		m_bFinish = false;
		m_FirstFlag = false;
		m_CamCommands.pop_front();
		//m_CamMode = m_NextCamMode;
	}
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	LOG_F(LOG_LEVEL::INFO,"Cam Pos : % f, % f, % f\n", XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
}

//void CCamera_Play::Change_CamMode(CAM_MODE m_ChangeMode, CAM_MODE m_NextMode)
//{
//	m_bChange = true;
//	m_ReservCamMode = m_ChangeMode;
//	m_ReservNextCamMode = m_NextMode;
//}


void CCamera_Play::Update_Follow(_float fTimeDelta, shared_ptr<CAM_DESC>pDesc)
{

	CDInput_Manager* dinput = m_pGameInstance.lock()->Get_DInput_Manger();
	_long dx = dinput->Get_DIMouseMove(DIMM::X);
	_long dy = dinput->Get_DIMouseMove(DIMM::Y);


	if (m_FirstFlag == false)
	{
	auto pFollowDesc = dynamic_pointer_cast<CAM_FOLLOW_DESC>(pDesc);
	if (!pFollowDesc) return;

	
	_vector vTargetPos = m_pTargetTransform.lock()->Get_Position() + XMVectorSet(0.f, 2.0f, 0.f, 0.f);
	_vector vCurrentPos = m_pTransformCom->Get_Position();
	_vector vDir = vCurrentPos - vTargetPos;

	_float3 fDir;
	XMStoreFloat3(&fDir, XMVector3Normalize(vDir));

	// 역계산
	m_Yaw = XMConvertToDegrees(atan2f(fDir.x, fDir.z)) - 180.f;
	m_Pitch = XMConvertToDegrees(asinf(fDir.y));
	m_fDistance = XMVectorGetX(XMVector3Length(vDir));


	
	m_FirstFlag = true;
	return ;
	}



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
			m_Yaw += delta * 5.f * fTimeDelta; // 추적 속도 (조절 가능)
		}
		m_Yaw = NormalizeAngle(m_Yaw);


		m_Pitch = clamp(m_Pitch, 0.f, 89.f);
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

bool CCamera_Play::Update_Lerp(_float fTimeDelta, shared_ptr<CAM_DESC>pDesc)
{
	// 이넘으로 타입 확실한데 스테틱으로 나중에
	auto pLerpDesc = dynamic_pointer_cast<CAM_LERP_DESC>(pDesc);
	if (!pLerpDesc) return false;

	if (m_FirstFlag == false)
	{
		_vector vTargetPos = XMLoadFloat3(&pLerpDesc->vTargetPos);
		m_pTransformCom->Start_Lerp(vTargetPos, pLerpDesc->vTargetRot, pLerpDesc->fDuration);
		m_FirstFlag = true;
		return true;
	}




	m_pTransformCom->Lerp_To(fTimeDelta);


		if(auto pTarget = pLerpDesc->m_Target.lock())
		{
			m_pTransformCom->LookAt(pTarget->Get_TransformCom()->Get_Position());
		}

	__super::Update_TransformMatrices();
	if (m_pTransformCom->GetIsLerp() == false)
	{
		m_FirstFlag = false;
		return false;
	}





	return true;

}

void CCamera_Play::SetTarget(weak_ptr<CGameObject> target, _float startPitchAngle, _float startYawAngle, _float startDistance, _float fTimeDelta)
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
