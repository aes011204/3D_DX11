#include "Player_Sea.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"

CPlayer_Sea::CPlayer_Sea(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_Sea::~CPlayer_Sea()
{
}

void CPlayer_Sea::Enter()
{
	CPlayerState::Enter();
}

void CPlayer_Sea::Exit()
{
	CPlayerState::Exit();
}

HRESULT CPlayer_Sea::Init_State()
{
	m_Acceleration = 2.f;
	m_Deceleration = 1.f;
	m_MaxSpeed = 5.f;
	m_pSea_Manager = CSea_Manager::GetInstance();

	return CPlayerState::Init_State();
}

int CPlayer_Sea::Update_State(const _float& timeDelta)
{
	Location_Sea(timeDelta);

	return ETOI(PLAYERSTATE::SEA);
}

void CPlayer_Sea::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_Sea::Render_State()
{
	
}

void CPlayer_Sea::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	// 돌이랑 충돌하면 0 아님 - 로 _CurrentSpeed
}

void CPlayer_Sea::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	
}

void CPlayer_Sea::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	// 오버랩 되어있고 키F를 누르고 있으면 지정된 위치로 천천이 이동 // 정박 -> 끝나면 Village

}


void CPlayer_Sea::Location_Sea(_float fTimeDelta)
{
	auto m_pTransformCom = m_pOwnerTransformCom.lock();
	auto dinput = m_Input_Manager;

	_float4 upDir = { 0.f, 1.f, 0.f, 0.f };
	bool isInput = false;

	if (dinput->KeyPress(DIK_UP))
	{
		m_CurSpeed += m_Acceleration * fTimeDelta;
		isInput = true;
	}
	 if (dinput->KeyPress(DIK_DOWN))
	{
		m_CurSpeed -= m_Acceleration * fTimeDelta;
		isInput = true;
	}
	 if (!isInput)
	 {

		if(m_CurSpeed > 0.f)
		{
			m_CurSpeed -= m_Deceleration* fTimeDelta;
			if (m_CurSpeed < 0.f)
				m_CurSpeed = 0.f;
		}
		if (m_CurSpeed < 0.f)
		{
			m_CurSpeed += m_Deceleration* fTimeDelta;
			if (m_CurSpeed > 0.f)
				m_CurSpeed = 0.f;
		}
	}
	//m_CurSpeed = clamp(m_CurSpeed, -m_MaxSpeed * 0.5f, m_MaxSpeed);
	float move = m_CurSpeed * fTimeDelta;
	if (m_CurSpeed > 0.f)
	{
		//m_pTransformCom->Set_Speed(m_CurSpeed);
		m_pTransformCom->Go_Forward((move));
		
	}
	if (m_CurSpeed < 0.f)
	{
		//m_pTransformCom->Set_Speed(abs(m_CurSpeed));
		m_pTransformCom->Go_Backward(abs(move));

	}


	if (dinput->KeyPress(DIK_RIGHT))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir), fTimeDelta);
	}

	if (dinput->KeyPress(DIK_LEFT))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir), -fTimeDelta);
	}




	
	auto Sea = m_pSea_Manager.lock();
	_vector CurPos = m_pTransformCom->Get_Position();

	//중점으로 y 위치
	//{
	_float fFinalPosY = {};

	//m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fFinalPosY);
	_float3 fianlPos = { XMVectorGetX(CurPos), Sea->Get_GlobalY(), XMVectorGetZ(CurPos) };
	m_pTransformCom->Set_Position(XMLoadFloat3(&fianlPos));
	//	
	//}


	// 4점 으로 기울기 + 보간
	{
		//_float3 FRBL[4] = { { 0.f,  0.f,1.f }, { 0.5f,0.f,0.f} , { 0.f, 0.f,-1.f }, { -0.5f, 0.f, 0.f, } };
		_float3 FRBL[4] = {};
		XMStoreFloat3(&FRBL[0], XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.5f);
		XMStoreFloat3(&FRBL[1], XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)) * 0.5f);
		XMStoreFloat3(&FRBL[2], XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * -1.5f);
		XMStoreFloat3(&FRBL[3], XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)) * -0.5f);

		_float3 Pos[4];
		Pos[0] = fianlPos + FRBL[0];
		Pos[1] = fianlPos + FRBL[1];
		Pos[2] = fianlPos + FRBL[2];
		Pos[3] = fianlPos + FRBL[3];

		_float3 fianlPosFRBL[4];
		for (_uint i = 0; i < 4; i++)
		{

			_float fOut1 = Sea->Calculate_GerstnerWave_Overlap(Pos[i]);

			fianlPosFRBL[i] = { Pos[i].x, fOut1, Pos[i].z };
		}

		_vector forwordDir = XMLoadFloat3(&fianlPosFRBL[0]) - XMLoadFloat3(&fianlPosFRBL[2]);
		_vector RightDir = XMLoadFloat3(&fianlPosFRBL[1]) - XMLoadFloat3(&fianlPosFRBL[3]);

		forwordDir = XMVector3Normalize(forwordDir);
		RightDir = XMVector3Normalize(RightDir);

		_vector FinalUpDir = XMVector3Normalize(XMVector3Cross(forwordDir, RightDir));

		_vector vOldShipForward = m_pTransformCom->Get_State(STATE::LOOK);

		_vector FinalRightDir = XMVector3Normalize(XMVector3Cross(FinalUpDir, vOldShipForward));

		_vector FinalLookDir = XMVector3Normalize(XMVector3Cross(FinalRightDir, FinalUpDir));

		_matrix NewRotationMatrix;
		NewRotationMatrix.r[0] = FinalRightDir;
		NewRotationMatrix.r[1] = FinalUpDir;
		NewRotationMatrix.r[2] = FinalLookDir;
		NewRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

		_vector targetQuat = XMQuaternionRotationMatrix(NewRotationMatrix);
		targetQuat = XMQuaternionNormalize(targetQuat);
		_vector currentQuat = m_pTransformCom->Get_Quaternion();

		_vector smoothQuat = XMQuaternionSlerp(currentQuat, targetQuat, 4.f * fTimeDelta);

		m_pTransformCom->Set_Quaternion(smoothQuat);
	}


	/*_vector CurPo3 = m_pTransformCom->Get_Position();
	_float fOut3 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fOut3);*/

	m_pTransformCom->Update_WorldMatrix();


	//m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	
}

shared_ptr<CPlayer_Sea> CPlayer_Sea::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_Sea> pInstance(new CPlayer_Sea(owner,pStateMachine), [](CPlayer_Sea* p) {p->Free(); delete(p); });

	//if (FAILED(pInstance->Init_State()))
	//{
	//	MSG_BOX("Failed to Created : CUIPanel");
	//
	//}
	return pInstance;
}

void CPlayer_Sea::Free()
{
	CPlayerState::Free();
}
