#include "Player_Sea.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "PlayerStateMachine.h"
#include "Sea_Manager.h"
#include "Camera_Play.h"
#include "EventBus.h"
#include "Fish.h"
#include "UI_Item.h"
#include "UI_TabContainer.h"

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
	// 만일 카메라 기본 위치와 현제 위치가 다르다면 럴프
	Evt_ChangeCam event = {};
	
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = LERP;
	_vector forward = m_pOwnerTransformCom.lock()->Get_State(STATE::LOOK);
	pLerp->IsLerpTarget = true;
	pLerp->fFov = 60;
	//pLerp->m_Target = m_Owner;
	pLerp->fDuration = 1.f;
	event.commands.push_back(pLerp);

	auto pFow = make_shared<CAM_FOLLOW_DESC>();
	pFow->eMode = CAM_MODE::FOLLOW;
	pFow->fPitch = 0.f;
	pFow->fYaw = 0.f;

	event.commands.push_back(pFow);

	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

}

void CPlayer_Sea::Exit()
{
	m_NextState = E_PLAYERSTATE::SEA;
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

	if (Move(timeDelta) == ETOI(E_PLAYERSTATE::VILLAGE))
		return ETOI(E_PLAYERSTATE::VILLAGE);
	Location_Sea(timeDelta);




	if(m_Input_Manager->KeyDown(DIK_F) && m_pStateMachine.lock()->Get_TargetShared()!= nullptr)
	{

		if(auto Target =dynamic_pointer_cast<CFish>(m_pStateMachine.lock()->Get_TargetShared()))
		{
			Target->Change_Cam(m_Owner.lock());

				m_CurSpeed = 0.f;


				Evt_FishingData e = {};
				e.Fish_ID = Target->Get_fish_DefID();
				e.Fish = Target;
				e.RodSpeed = m_Owner.lock()->Get_RodSpeed();
				m_pGameInstance.lock()->Get_EventBus()->Publish(e);


				return ETOI(E_PLAYERSTATE::FISHING);
		}

	}


	if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_TAB)) 
	{
		if (m_OnTab == false) 
		{
			m_TapUI = dynamic_pointer_cast<CUI_TabContainer>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer"));
			m_TapUI.lock()->UI_PanelActive(ETOI(TAB::INVEN), TAB::INVEN);

			auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
			m_HoldItem->UI_Active();


			m_OnTab = true;
		}
		else
		{
			m_TapUI.lock()->UI_InActive();

			m_OnTab = false;
		}
	}


	return ETOI(m_NextState);
}

void CPlayer_Sea::LateUpdate_State(const _float& timeDelta)
{

}

void CPlayer_Sea::Render_State()
{

}

void CPlayer_Sea::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	
}

void CPlayer_Sea::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
	m_pStateMachine.lock()->Set_Target(nullptr);
}

void CPlayer_Sea::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{

	if (other->Get_MyLayer() == COLLISION_LAYER::FISH)
	{
		
		m_pStateMachine.lock()->Set_Target(other->Get_GOwner());// = other->Get_GOwner();
	}
	
	if (m_Input_Manager->KeyDown(DIK_F) && !m_bIsDocking && other->Get_MyLayer() == TRIGGER)
	{
		_float3 vTargetCenter = other->Get_WorldCenter();

		m_pOwnerTransformCom.lock()->Start_Lerp(
			XMLoadFloat3(&vTargetCenter),
			_float3(0.f, -180.f, 0.f),
			2.f
		);


		Evt_ChangeCam event = {};
		auto pLerp = make_shared<CAM_DESC>();
		pLerp->eMode = CAM_MODE::STOP;
		
		event.commands.push_back(pLerp);

		CGameInstance::GetInstance()->Get_EventBus()->Publish(event);


		m_bIsDocking = true;
	}

	if(m_Input_Manager->KeyUp(DIK_F))
	{
		Evt_ChangeCam event = {};
		auto pfollow = make_shared<CAM_DESC>();
		pfollow->eMode = CAM_MODE::FOLLOW;

		event.commands.push_back(pfollow);

		CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
	}

	if (other->Get_MyLayer() == COLLISION_LAYER::ITEM)
	{
		if (m_Input_Manager->KeyDown(DIK_F))
		{
			m_NextState = E_PLAYERSTATE::INTERACT;
		}
	}


}


void CPlayer_Sea::Location_Sea(_float fTimeDelta)
{
	auto m_pTransformCom = m_pOwnerTransformCom.lock();
	
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

		/*m_pTransformCom->Set_Quaternion(smoothQuat);*/
		if (!m_bIsDocking)
		{
			m_pTransformCom->Set_Quaternion(smoothQuat);
		}
	}


	/*_vector CurPo3 = m_pTransformCom->Get_Position();
	_float fOut3 = {};
	m_pGameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fOut3);*/

	m_pTransformCom->Update_WorldMatrix();


	//m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}


_uint CPlayer_Sea::Move(_float fTimeDelta)
{
	//if (m_bIsDocking == true)
	//{
	//	m_pOwnerTransformCom.lock()->Lerp_To(fTimeDelta);
	//	if (m_pOwnerTransformCom.lock()->GetIsLerp() == false)
	//	{
	//		m_bFinDock = true;
	//		//m_bIsDocking = false;

	//	}
	//	return;
	//}

	if (m_bIsDocking)
	{
		float dt = 0.f;

		if (m_Input_Manager->KeyPress(DIK_F))
			dt = fTimeDelta;   // 누르면 진행
		else
			dt = 0.f;          // 떼면 멈춤

		m_pOwnerTransformCom.lock()->Lerp_To(dt);

		if (m_pOwnerTransformCom.lock()->GetIsLerp() == false)
		{
			/*Evt_ChangeCam event = {};
					auto pLerp = make_shared<CAM_LERP_DESC>();
					pLerp->eMode = CAM_MODE::LERP ;
					pLerp->vTargetPos = _float3(8.2f, 3.f, 7.6f);
					pLerp->vTargetRot = _float3(5.f, -130.f, 0.f);
					pLerp->fDuration = 2.0f;
					
					event.commands.push_back(pLerp);

					CGameInstance::GetInstance()->Get_EventBus()->Publish(event);*/

					//m_pStateMachine.lock()->Change_State(ETOI(E_PLAYERSTATE::VILLAGE));
					m_CurSpeed = 0.f;
			m_bFinDock = true;
			m_bIsDocking = false;
			/*m_NextState = E_PLAYERSTATE::VILLAGE;*/
			return ETOI(E_PLAYERSTATE::VILLAGE);
		}
		return ETOI(E_PLAYERSTATE::SEA);
	}
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

		if (m_CurSpeed > 0.f)
		{
			m_CurSpeed -= m_Deceleration * fTimeDelta;
			if (m_CurSpeed < 0.f)
				m_CurSpeed = 0.f;
		}
		if (m_CurSpeed < 0.f)
		{
			m_CurSpeed += m_Deceleration * fTimeDelta;
			if (m_CurSpeed > 0.f)
				m_CurSpeed = 0.f;
		}
	}
	m_CurSpeed = clamp(m_CurSpeed, -m_MaxSpeed * 0.5f, m_MaxSpeed);
	float move = m_CurSpeed * fTimeDelta;
	if (m_CurSpeed > 0.f)
	{

		m_pTransformCom->Go_Forward_Distanace((move));

	}
	if (m_CurSpeed < 0.f)
	{

		m_pTransformCom->Go_Backward_Distanace(abs(move));

	}


	if (dinput->KeyPress(DIK_RIGHT))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir), fTimeDelta);
	}

	if (dinput->KeyPress(DIK_LEFT))
	{
		m_pTransformCom->Turn(XMLoadFloat4(&upDir), -fTimeDelta);
	}

return 	ETOI(E_PLAYERSTATE::SEA);
}

shared_ptr<CPlayer_Sea> CPlayer_Sea::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_Sea> pInstance(new CPlayer_Sea(owner, pStateMachine), [](CPlayer_Sea* p) {p->Free(); delete(p); });

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
