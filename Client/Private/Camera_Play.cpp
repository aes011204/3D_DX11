#include "Camera_Play.h"
#include "DInput_Manager.h"
#include "Transform.h"
#include "EventBus.h"
#include <cmath>

CCamera_Play::CCamera_Play(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CCamera(device, context)
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

HRESULT CCamera_Play::Initialize(void* arg)
{
	CameraPlayDesc* desc = static_cast<CameraPlayDesc*>(arg);
	m_mouseSensor = desc->mouseSensor;
	m_target = desc->target;

	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	m_targetTransform = dynamic_pointer_cast<CTransform>(m_target.lock()->Get_Component(g_strTransformTag));

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeCam>(
		[this](const Evt_ChangeCam& e)
		{
			m_camCommands = e.commands;
			//m_bChange = true;
			m_isFirstFlag = false;
			m_isFinish = false;
		}
	);

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_Damage>(
		[this](const Evt_Damage& e)
		{
			if (m_isShack) return;

			m_shakePower = e.shakePower;
			m_shakeTime = e.shakeTime;
			m_shakeDecay = e.shakePower / e.shakeTime;;


			m_isShack = true;

		}
	);
	m_mouseSensor = 0.2f;


	m_minPitch = 5.f;
	m_maxPitch = 80.f;

	m_minDistance = 7.f;
	m_maxDistance = 20.f;
	//m_MinDistance = 15.f;
	//m_MaxDistance = 35.f;
	m_distance = 18.f;



	//_pTransformCom->Set_RotationDegree(pDesc->vDir);
	//_pTransformCom->Update_WorldMatrix();
	return S_OK;
}

void CCamera_Play::Priority_Update(_float timeDelta)
{
}

void CCamera_Play::Update_AfterTarget(_float timeDelta)
{
	if (m_camCommands.empty())
		return;

	/*if (m_bChange == true)
	{
		m_CamMode = m_ReservCamMode;
		m_NextCamMode = m_ReservNextCamMode;
		m_bChange = false;
	}*/
	auto currentDesc = m_camCommands.front();
	ECamMode mode = currentDesc->mode;
	switch (mode)
	{
	case ECamMode::FOLLOW:
		Update_Follow(timeDelta, currentDesc);

		break;
	case ECamMode::STOP:
	//	__super::Update_TransformMatrices();
		break;
	case ECamMode::LERP:
		if (false == Update_Lerp(timeDelta, currentDesc))
			m_isFinish = true;


		break;

	case ECamMode::END:
		break;
	}




	XMStoreFloat3(&m_basePos, m_transformCom->Get_Position());

	Shake_Cam(timeDelta);


	// 4. 최종 행렬 업데이트
	__super::Update_TransformMatrices();


	if (m_isFinish == true)
	{
		if(currentDesc->OnComplete != nullptr)
		{
			currentDesc->OnComplete();
		}
		m_isFinish = false;
		m_isFirstFlag = false;
		m_camCommands.pop_front();
		//m_CamMode = m_NextCamMode;

	}
	//_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	//LOG_F(LOG_LEVEL::INFO,"Cam Pos : % f, % f, % f\n", XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));


}

//void CCamera_Play::Change_CamMode(CAM_MODE m_ChangeMode, CAM_MODE m_NextMode)
//{
//	m_bChange = true;
//	m_ReservCamMode = m_ChangeMode;
//	m_ReservNextCamMode = m_NextMode;
//}


void CCamera_Play::Update_Follow(_float timeDelta, shared_ptr<CamDesc>desc)
{

	CDInput_Manager* dinput = m_gameInstance.lock()->Get_DInput_Manger();
	_long dx = dinput->Get_DIMouseMove(EMouseMove::X);
	_long dy = dinput->Get_DIMouseMove(EMouseMove::Y);


	if (m_isFirstFlag == false)
	{
		auto followDesc = dynamic_pointer_cast<CamFollowDesc>(desc);
		if (!followDesc) return;


		_vector vecTargetPos = m_targetTransform.lock()->Get_Position() + XMVectorSet(0.f, 2.0f, 0.f, 0.f);
		m_followTargetY = XMVectorGetY(vecTargetPos);
		_vector vecCurrentPos = m_transformCom->Get_Position();
		_vector vecDir = vecCurrentPos - vecTargetPos;

		_float3 dir;
		XMStoreFloat3(&dir, XMVector3Normalize(vecDir));

		// 역계산
		m_yaw = XMConvertToDegrees(atan2f(dir.x, dir.z)) - 180.f;
		m_pitch = XMConvertToDegrees(asinf(dir.y));
		m_distance = XMVectorGetX(XMVector3Length(vecDir));


		m_isFirstFlag = true;
		return;
	}



	// 타겟을 항상 보고있다
	if (auto target = m_target.lock())
	{

		_vector targetLook = m_targetTransform.lock()->Get_State(ETransformState::LOOK);
		_float3 look;
		XMStoreFloat3(&look, XMVector3Normalize(targetLook));


		float targetDegree = XMConvertToDegrees(atan2f(look.x, look.z));


		m_pitch += dy * m_mouseSensor;
		m_yaw += dx * m_mouseSensor;

		auto NormalizeAngle = [](float angle) {
			while (angle > 180.f) angle -= 360.f;
			while (angle < -180.f) angle += 360.f;
			return angle;
			};

		m_yaw = NormalizeAngle(m_yaw);
		targetDegree = NormalizeAngle(targetDegree);
		//targetDegree = m_Yaw;

		if (dx == 0 && dy == 0)
		{
			float delta = NormalizeAngle(targetDegree - m_yaw);
			m_yaw += delta * 2.f * timeDelta; // 추적 속도 (조절 가능)
		}
		m_yaw = NormalizeAngle(m_yaw);


		m_pitch = clamp(m_pitch, 1.f, 80.f);
		_float normalizePitch = (m_pitch - m_minPitch) / (m_maxPitch - m_minPitch);
		m_distance = lerp(m_minDistance, m_maxDistance, normalizePitch);


		float yawRad = XMConvertToRadians(m_yaw + 180.f);
		float pitchRad = XMConvertToRadians(m_pitch);

		float x = m_distance * cosf(pitchRad) * sinf(yawRad);
		float y = m_distance * sinf(pitchRad);
		float z = m_distance * cosf(pitchRad) * cosf(yawRad);

		_vector offset = XMVectorSet(x, y, z, 0.f);


		_vector targetPos = m_targetTransform.lock()->Get_Position() + XMVectorSet(0.f, 2.0f, 0.f, 0.f);
		constexpr float heightResponse = 2.f;
		const float heightFollow = 1.f - std::exp(-heightResponse * max(timeDelta, 0.f));
		m_followTargetY += (XMVectorGetY(targetPos) - m_followTargetY) * heightFollow;
		targetPos = XMVectorSetY(targetPos, m_followTargetY);
		_vector camPos = targetPos + offset;


		_vector currentPos = m_transformCom->Get_Position();
		_vector newPos = XMVectorLerp(currentPos, camPos, 8.f * timeDelta);

		m_transformCom->Set_Position(newPos);
		//XMStoreFloat3(&m_FinalPos, newPos);
		m_transformCom->LookAt(targetPos);

	}
	//__super::Update_TransformMatrices();
}

bool CCamera_Play::Update_Lerp(_float timeDelta, shared_ptr<CamDesc>desc)
{
	//Lerp 일떄는 카메라 쉐이크 없음 -> 이거는 Transform //

	// 이넘으로 타입 확실한데 스테틱으로 나중에
	auto lerpDesc = dynamic_pointer_cast<CamLerpDesc>(desc);
	if (!lerpDesc) return false;

	if (m_isFirstFlag == false)
	{
		if(lerpDesc->isLerpTarget ==true)
		{
			_float3 cam, angle;
			Get_Target_PosLook(cam, angle);
			lerpDesc->targetPos = cam;
				lerpDesc->targetRot = angle;
		}
		_vector vecTargetPos = XMLoadFloat3(&lerpDesc->targetPos);

		m_transformCom->Start_Lerp(vecTargetPos, lerpDesc->targetRot, lerpDesc->duration);
		m_isFirstFlag = true;

		m_startFOVY = m_fovy;

		return true;
	}



	m_transformCom->Lerp_To(timeDelta);

	m_acc += timeDelta;
	if (m_acc >= lerpDesc->duration)
	{
		m_acc = lerpDesc->duration;
	}


	if (lerpDesc->FOV > 0)
	{
		m_fovy = lerp(m_startFOVY, XMConvertToRadians(lerpDesc->FOV) , m_acc / lerpDesc->duration);
	}



		if(auto target = lerpDesc->target.lock())
		{
			m_transformCom->LookAt(target->Get_TransformCom()->Get_Position());
		}

	//__super::Update_TransformMatrices();
	if (m_transformCom->GetIsLerp() == false)
	{
		m_acc = 0.f;
		m_isFirstFlag = false;
		return false;
	}





	return true;

}

void CCamera_Play::Get_Target_PosLook(_float3& camPos, _float3& targetRot)
{
	//_vector targetLook = m_pTargetTransform.lock()->Get_State(STATE::LOOK);
	//_float3 vLook;
	//XMStoreFloat3(&vLook, XMVector3Normalize(targetLook));

	//float targetDegree = XMConvertToDegrees(atan2f(vLook.x, vLook.z));


	//m_Pitch = 30.f;
	//	m_Yaw = targetDegree;

	//m_Pitch = clamp(m_Pitch, 0.f, 89.f);
	//_float normalizePitch = (m_Pitch - m_MinPitch) / (m_MaxPitch - m_MinPitch);
	//m_fDistance = lerp(m_MinDistance, m_MaxDistance, normalizePitch);


	//float yawRad = XMConvertToRadians(m_Yaw + 180.f);
	//float pitchRad = XMConvertToRadians(m_Pitch);

	//float x = m_fDistance * cosf(pitchRad) * sinf(yawRad);
	//float y = m_fDistance * sinf(pitchRad);
	//float z = m_fDistance * cosf(pitchRad) * cosf(yawRad);

	//_vector offset = XMVectorSet(x, y, z, 0.f);


	//_vector targetPos = m_pTargetTransform.lock()->Get_Position() + XMVectorSet(0.f, 2.0f, 0.f, 0.f);
	//_vector camposVetor = targetPos + offset;
	//_float3 tmp = {};
	// XMStoreFloat3(&tmp, camposVetor);
	// camPos = tmp;
	//
	//	_vector currentPos = m_pTransformCom->Get_Position();

	//	_vector vDir = XMVector3Normalize(targetPos - camPos);
	//	_float3 fDir;
	//	XMStoreFloat3(&fDir, vDir);


	//	float finalYaw = XMConvertToDegrees(atan2f(fDir.x, fDir.z));
	//	float finalPitch = XMConvertToDegrees(asinf(fDir.y));
	//	float finalRoll = 0.f;

	//	vTargetRot = _float3(finalPitch, finalYaw, finalRoll);
	//

	_vector targetLook = m_targetTransform.lock()->Get_State(ETransformState::LOOK);
	_float3 look;
	XMStoreFloat3(&look, XMVector3Normalize(targetLook));
	float targetDegree = XMConvertToDegrees(atan2f(look.x, look.z));

	
	m_pitch = 30.f;
	m_yaw = targetDegree;

	
	m_pitch = clamp(m_pitch, 0.f, 89.f);
	_float normalizePitch = (m_pitch - m_minPitch) / (m_maxPitch - m_minPitch);
	m_distance = lerp(m_minDistance, m_maxDistance, normalizePitch);

	float yawRad = XMConvertToRadians(m_yaw + 180.f); // 배의 뒤쪽 좌표를 구하기 위해 180도 더함
	float pitchRad = XMConvertToRadians(m_pitch);

	float x = m_distance * cosf(pitchRad) * sinf(yawRad);
	float y = m_distance * sinf(pitchRad);
	float z = m_distance * cosf(pitchRad) * cosf(yawRad);

	_vector offset = XMVectorSet(x, y, z, 0.f);
	_vector targetPos = m_targetTransform.lock()->Get_Position() + XMVectorSet(0.f, 2.0f, 0.f, 0.f);

	
	_vector camposVetor = targetPos + offset;
	XMStoreFloat3(&camPos, camposVetor);


	targetRot = _float3(m_pitch, m_yaw, 0.f);
}

_float3 CCamera_Play::Shake_Cam(_float timeDelta)
{

	if(m_isShack == false)
		return _float3{ 0.f, 0.f, 0.f };


	if(m_shakeTime > 0.f)
	{
		m_shakeTime -= timeDelta;

		float randX = m_gameInstance.lock()->Random(-1.f, 1.f) * m_shakePower;
		float randY = m_gameInstance.lock()->Random(-1.f, 1.f) * m_shakePower;

		m_shakePower -= timeDelta  * (m_shakeDecay);

		_float3 finalPos = _float3{ randX,randY,0.f };

		if (m_shakeTime <= 0)
			m_isShack = false;

		m_transformCom->Set_Position(m_basePos + _float3{ randX,randY,0.f });
		return _float3{ randX, randY, 0.f }; // 오프셋만 반환
	}
	return _float3{ 0.f, 0.f, 0.f };


}

void CCamera_Play::SetTarget(weak_ptr<CGameObject> target, _float startPitchAngle, _float startYawAngle, _float startDistance, _float timeDelta)
{

}

void CCamera_Play::Update(_float timeDelta)
{
	// 레이충돌로 장애물이 있을경우 앞으로 당기던가 해야함
}

void CCamera_Play::Late_Update(_float timeDelta)
{
}

HRESULT CCamera_Play::Render()
{
	return S_OK;
}

void CCamera_Play::Start_Targetting(_float4 startPos, _float degree, _float distance)
{

}

shared_ptr<CCamera_Play> CCamera_Play::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CCamera_Play> instance(new CCamera_Play(device, context), [](CCamera* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Play");
	}
	return instance;
}

shared_ptr<CGameObject> CCamera_Play::Clone(void* arg)
{
	shared_ptr<CCamera_Play> instance(new CCamera_Play(*this), [](CCamera* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Play");
	}
	return instance;
}

void CCamera_Play::Free()
{
}
