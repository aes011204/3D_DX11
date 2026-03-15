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

	_long		MouseMoveX = {};
	_long		MouseMoveY = {};

	if (MouseMoveX = dinput->Get_DIMouseMove(DIMM::X))
	{
		// 누적 하면 됨
		//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
	}

	if (MouseMoveY = dinput->Get_DIMouseMove(DIMM::Y))
	{
		//m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
	}

	//if(dinput->Get_DIMouseMove(DIMM::X)==0&& dinput->Get_DIMouseMove(DIMM::Y) == 0)
	//{
	//	//if (auto pTarget = m_pTarget.lock())
	//	//{
	//	//	// 타겟살아있을떄

	//		// 0 이면  lerp 0으로 
	//	//}
	//}


	// 타겟을 항상 보고있다
	if (auto pTarget = m_pTarget.lock())
	{
		// 타겟살아있을떄
		m_pTransformCom->Orbit(m_pTargetTransform.lock()->Get_Position(), m_pTargetTransform.lock()->Get_Quaternion(), 20.f, MouseMoveY+45.f, MouseMoveX);

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
