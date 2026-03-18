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

    return S_OK;
}

void CCamera_Play::Priority_Update(_float fTimeDelta)
{
	// 움직임이 없으면 원래 상태로 돌아간다
	CDInput_Manager* dinput = m_pGameInstance.lock()->Get_DInput_Manger();


	_long dx = dinput->Get_DIMouseMove(DIMM::X);
	_long dy = dinput->Get_DIMouseMove(DIMM::Y);



	m_Yaw += dx;
	
	m_Pitch += dy;
	
	_float speed = 10.f;
	//m_time += fTimeDelta;
	if(dx ==0 && dy==0)
	{

		m_Yaw += (m_pTargetTransform.lock()->Get_RotationDegree().y - m_Yaw) * speed * fTimeDelta;
		//m_Pitch += (m_pTargetTransform.lock()->Get_RotationDegree().x - m_Pitch) * (speed/10) * fTimeDelta;
		//if (m_Yaw > 0)
		//	m_Yaw -= speed* fTimeDelta;
		//if (m_Yaw < 0)
		//	m_Yaw += speed* fTimeDelta;
		//
		//if (m_Pitch > 0)
		//	m_Pitch -= speed* fTimeDelta;
		//if (m_Pitch < 0)
		//	m_Pitch += speed* fTimeDelta;

	}
		//m_Yaw= clamp(m_Yaw, -90.f, 90.f);
		//m_Pitch = clamp(m_Pitch, -90.f, 90.f);


	// 타겟을 항상 보고있다
	if (auto pTarget = m_pTarget.lock())
	{
		// 타겟살아있을떄
		_float pitch = m_Pitch + 45.f;
		//pitch = max(5.f, pitch);

		float yawRad = XMConvertToRadians(m_Yaw +180.f);
		float pitchRad = XMConvertToRadians(m_Pitch +45.f);

		m_fDistance = 20.f;

		float x = m_fDistance * cosf(pitchRad) * sinf(yawRad);
		float y = m_fDistance * sinf(pitchRad);
		float z = m_fDistance * cosf(pitchRad) * cosf(yawRad);

		_vector offset = XMVectorSet(x, y, z, 0.f);

		_vector targetPos = m_pTargetTransform.lock()->Get_Position();

		_vector camPos = targetPos + offset;

		m_pTransformCom->Set_Position(camPos);

		//m_pTransformCom->Orbit(m_pTargetTransform.lock()->Get_Position(), m_pTargetTransform.lock()->Get_RotationDegree(), 20.f, pitch , m_Yaw);
		m_pTransformCom->LookAt(m_pTargetTransform.lock()->Get_Position());




		// 클램프 바다 밑으로 못들어가게

	}
	__super::Update_TransformMatrices();

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
