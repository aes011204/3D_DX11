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
	// 留뚯??移�?�??湲곕???꾩튂?? ?꾩젣 ?꾩튂媛 ??�Ⅴ??�㈃ ??�봽
	Evt_ChangeCam event = {};
	
	auto pLerp = make_shared<CamLerpDesc>();
	pLerp->eMode = ECamMode::LERP;
	_vector forward = m_ownerTransformCom.lock()->Get_State(ETransformState::LOOK);
	pLerp->IsLerpTarget = true;
	pLerp->fFov = 60;
	//pLerp->target = m_owner;
	pLerp->fDuration = 1.f;
	event.commands.push_back(pLerp);

	auto pFow = make_shared<CamFollowDesc>();
	pFow->eMode = ECamMode::FOLLOW;
	pFow->fPitch = 0.f;
	pFow->fYaw = 0.f;

	event.commands.push_back(pFow);

	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
	CGameInstance::GetInstance()->Set_TimeScale(2.f);

	m_acceleration = m_owner.lock()->Get_BoatSpeed() * 0.15f;
	m_deceleration = 1.f;
	m_maxSpeed = m_owner.lock()->Get_BoatSpeed() * 0.25f;

	m_curTurnSpeed = 0.f;
	m_maxTurnSpeed = XMConvertToRadians(40.f);
	m_turnAcceleration = m_maxTurnSpeed * 2.5f;
	m_turnDeceleration = m_maxTurnSpeed * 1.5f;

	m_gameInstance.lock()->Play_Loop(L"Waves_Ambience_1");
}

void CPlayer_Sea::Exit()
{
	m_nextState = EPlayerState::SEA;

	m_gameInstance.lock()->Stop(L"Waves_Ambience_1",2.f);
	CPlayerState::Exit();
}

HRESULT CPlayer_Sea::Init_State()
{

	m_seaManager = CSea_Manager::GetInstance();


	m_lightObj = m_owner.lock()->Get_Light();
	
	return CPlayerState::Init_State();
}

int CPlayer_Sea::Update_State(const _float& timeDelta)
{
	if (m_owner.lock() && m_owner.lock()->IsInputBlocked())
	{
		m_curSpeed = 0.f;
		m_curTurnSpeed = 0.f;
		Location_Sea(timeDelta);
		return ETOI(m_nextState);
	}

	if (Move(timeDelta) == ETOI(EPlayerState::VILLAGE))
		return ETOI(EPlayerState::VILLAGE);
	Location_Sea(timeDelta);


	if (m_inputManager->MouseDown(EMouseButton::RBUTTON) && m_lightObj.lock())
	{
		On_Light = !On_Light;
		m_lightObj.lock()->Set_Active(On_Light);

		{
			Evt_BoatLight e = {};
			e.Light = On_Light;
			m_gameInstance.lock()->Get_EventBus()->Publish<Evt_BoatLight>(e);
		}


		if(On_Light)
		m_gameInstance.lock()->Play_Once(L"light_on");
		else
			m_gameInstance.lock()->Play_Once(L"light_off");

	}
	if (m_lightObj.lock()->Get_Active() == true)
	{
		XMVECTOR vPos = m_ownerTransformCom.lock()->Get_Position();
		XMVECTOR vLook = XMVector3Normalize(m_ownerTransformCom.lock()->Get_State(ETransformState::LOOK));

		XMVECTOR vFinal = vPos + vLook * 1.5f + XMVectorSet(0.f, 0.9f, 0.f, 0.f);
		_float3 finalPos;
		XMStoreFloat3(&finalPos, vFinal);

		m_lightObj.lock()->Set_Position(finalPos.x, finalPos.y, finalPos.z);
	}



	if(m_inputManager->KeyDown(DIK_F) && m_stateMachine.lock()->Get_TargetShared()!= nullptr)
	{

		if(auto Target =dynamic_pointer_cast<CFish>(m_stateMachine.lock()->Get_TargetShared()))
		{
			Target->Change_Cam(m_owner.lock());

				m_curSpeed = 0.f;


				Evt_FishingData e = {};
				e.Fish_ID = Target->Get_fish_DefID();
				e.Fish = Target;
				e.RodSpeed = m_owner.lock()->Get_RodSpeed();
				m_gameInstance.lock()->Get_EventBus()->Publish(e);


				return ETOI(EPlayerState::FISHING);
		}

	}


	if (m_gameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_TAB)) 
	{
		if (m_isOnTab == false) 
		{
			m_tabUi = dynamic_pointer_cast<CUI_TabContainer>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer"));
			m_tabUi.lock()->UI_PanelActive(ETOI(ETab::INVEN), ETab::INVEN);

			auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
			m_holdItem->UI_Active();


			m_isOnTab = true;
		}
		else
		{
			m_tabUi.lock()->UI_InActive();

			m_isOnTab = false;
		}
	}


	return ETOI(m_nextState);
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
	m_stateMachine.lock()->Set_Target(nullptr);
}

void CPlayer_Sea::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{

	if (other->Get_MyLayer() == ECollisionLayer::FISH)
	{
		
		m_stateMachine.lock()->Set_Target(other->Get_GOwner());// = other->Get_GOwner();
	}
	
	if (m_inputManager->KeyDown(DIK_F) && !m_isDocking && other->Get_MyLayer() == TRIGGER)
	{
		_float3 vTargetCenter = other->Get_WorldCenter();

		m_ownerTransformCom.lock()->Start_Lerp(
			XMLoadFloat3(&vTargetCenter),
			_float3(0.f, -180.f, 0.f),
			2.f
		);


		Evt_ChangeCam event = {};
		auto pLerp = make_shared<CamDesc>();
		pLerp->eMode = ECamMode::STOP;
		
		event.commands.push_back(pLerp);

		CGameInstance::GetInstance()->Get_EventBus()->Publish(event);


		m_isDocking = true;
	}

	if(m_inputManager->KeyUp(DIK_F))
	{
		Evt_ChangeCam event = {};
		auto pfollow = make_shared<CamDesc>();
		pfollow->eMode = ECamMode::FOLLOW;

		event.commands.push_back(pfollow);

		CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
	}

	if (other->Get_MyLayer() == ECollisionLayer::ITEM)
	{
		if (m_inputManager->KeyDown(DIK_F))
		{
			m_nextState = EPlayerState::INTERACT;
		}
	}


}


void CPlayer_Sea::Location_Sea(_float fTimeDelta)
{
	auto m_transformCom = m_ownerTransformCom.lock();
	
	auto Sea = m_seaManager.lock();
	_vector CurPos = m_transformCom->Get_Position();

	//以묒???�줈 y ?꾩튂
	//{
	_float fFinalPosY = {};

	//m_gameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fFinalPosY);
	_float3 fianlPos = { XMVectorGetX(CurPos), Sea->Get_GlobalY(), XMVectorGetZ(CurPos) };
	fianlPos.y = Sea->Calculate_GerstnerWavePosition(fianlPos).y;
	m_transformCom->Set_Position(XMLoadFloat3(&fianlPos));
	//	
	//}


	// 4????�줈 湲곗?�湲?+ 蹂닿�?
	{
		//_float3 FRBL[4] = { { 0.f,  0.f,1.f }, { 0.5f,0.f,0.f} , { 0.f, 0.f,-1.f }, { -0.5f, 0.f, 0.f, } };
		_float3 FRBL[4] = {};
		_vector flatLook = XMVectorSetY(m_transformCom->Get_State(ETransformState::LOOK), 0.f);
		flatLook = XMVector3Normalize(flatLook);
		const _vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector flatRight = XMVector3Normalize(XMVector3Cross(worldUp, flatLook));

		XMStoreFloat3(&FRBL[0], flatLook * 1.5f);
		XMStoreFloat3(&FRBL[1], flatRight * 0.5f);
		XMStoreFloat3(&FRBL[2], flatLook * -1.5f);
		XMStoreFloat3(&FRBL[3], flatRight * -0.5f);

		_float3 Pos[4];
		Pos[0] = fianlPos + FRBL[0];
		Pos[1] = fianlPos + FRBL[1];
		Pos[2] = fianlPos + FRBL[2];
		Pos[3] = fianlPos + FRBL[3];

		_float3 fianlPosFRBL[4];
		for (_uint i = 0; i < 4; i++)
		{

			fianlPosFRBL[i] = Sea->Calculate_GerstnerWavePosition(Pos[i]);
		}

		_vector forwordDir = XMLoadFloat3(&fianlPosFRBL[0]) - XMLoadFloat3(&fianlPosFRBL[2]);
		_vector RightDir = XMLoadFloat3(&fianlPosFRBL[1]) - XMLoadFloat3(&fianlPosFRBL[3]);

		forwordDir = XMVector3Normalize(forwordDir);
		RightDir = XMVector3Normalize(RightDir);

		_vector FinalUpDir = XMVector3Normalize(XMVector3Cross(forwordDir, RightDir));

		_vector FinalRightDir = XMVector3Normalize(XMVector3Cross(FinalUpDir, flatLook));

		_vector FinalLookDir = XMVector3Normalize(XMVector3Cross(FinalRightDir, FinalUpDir));

		_matrix NewRotationMatrix;
		NewRotationMatrix.r[0] = FinalRightDir;
		NewRotationMatrix.r[1] = FinalUpDir;
		NewRotationMatrix.r[2] = FinalLookDir;
		NewRotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

		_vector targetQuat = XMQuaternionRotationMatrix(NewRotationMatrix);
		targetQuat = XMQuaternionNormalize(targetQuat);
		_vector currentQuat = m_transformCom->Get_Quaternion();

		_vector smoothQuat = XMQuaternionSlerp(currentQuat, targetQuat, 4.f * fTimeDelta);

		/*m_transformCom->Set_Quaternion(smoothQuat);*/
		if (!m_isDocking)
		{
			m_transformCom->Set_Quaternion(smoothQuat);
		}
	}


	/*_vector CurPo3 = m_transformCom->Get_Position();
	_float fOut3 = {};
	m_gameInstance.lock()->Compute_HeightOnTerrain(CurPos, &fOut3);*/

	m_transformCom->Update_WorldMatrix();


	//m_colliderCom->Update(XMLoadFloat4x4(m_transformCom->Get_WorldMatrixPtr()));

}


_uint CPlayer_Sea::Move(_float fTimeDelta)
{
	//if (m_isDocking == true)
	//{
	//	m_ownerTransformCom.lock()->Lerp_To(fTimeDelta);
	//	if (m_ownerTransformCom.lock()->GetIsLerp() == false)
	//	{
	//		m_isFinDock = true;
	//		//m_isDocking = false;

	//	}
	//	return;
	//}

	if (m_isDocking)
	{
		bool isPress = m_inputManager->KeyPress(DIK_F);
		float dt = 0.f;
		if (isPress)
		{
			if (!bPlaying)
			{
				m_gameInstance.lock()->Play_Loop(L"Dock_Progress");
				bPlaying = true;
			}

			dt = fTimeDelta;
		}
		else
		{
			if (bPlaying)
			{
				m_gameInstance.lock()->Stop(L"Dock_Progress");
				bPlaying = false;
			}

			dt = 0.f;
		}



		m_ownerTransformCom.lock()->Lerp_To(dt);

		if (m_ownerTransformCom.lock()->GetIsLerp() == false)
		{
			/*Evt_ChangeCam event = {};
					auto pLerp = make_shared<CamLerpDesc>();
					pLerp->eMode = ECamMode::LERP ;
					pLerp->vTargetPos = _float3(8.2f, 3.f, 7.6f);
					pLerp->vTargetRot = _float3(5.f, -130.f, 0.f);
					pLerp->fDuration = 2.0f;
					
					event.commands.push_back(pLerp);

					CGameInstance::GetInstance()->Get_EventBus()->Publish(event);*/

					//m_stateMachine.lock()->Change_State(ETOI(EPlayerState::VILLAGE));
					m_curSpeed = 0.f;
			m_isFinDock = true;
			m_isDocking = false;
			isPress = false;
			m_gameInstance.lock()->Stop(L"Dock_Progress");
			m_gameInstance.lock()->Play_Once(L"Docked");

			/*m_nextState = EPlayerState::VILLAGE;*/
			return ETOI(EPlayerState::VILLAGE);
		}
		return ETOI(EPlayerState::SEA);
	}
	auto m_transformCom = m_ownerTransformCom.lock();
	auto dinput = m_inputManager;

	_float4 upDir = { 0.f, 1.f, 0.f, 0.f };
	bool isInput = false;

	if (dinput->KeyPress(DIK_W))
	{
		m_curSpeed += m_acceleration * fTimeDelta;
		isInput = true;
	}
	if (dinput->KeyPress(DIK_S))
	{
		m_curSpeed -= m_acceleration * fTimeDelta;
		isInput = true;
	}
	if (!isInput)
	{

		if (m_curSpeed > 0.f)
		{
			m_curSpeed -= m_deceleration * fTimeDelta;
			if (m_curSpeed < 0.f)
				m_curSpeed = 0.f;
		}
		if (m_curSpeed < 0.f)
		{
			m_curSpeed += m_deceleration * fTimeDelta;
			if (m_curSpeed > 0.f)
				m_curSpeed = 0.f;
		}
	}
	m_curSpeed = clamp(m_curSpeed, -m_maxSpeed * 0.5f, m_maxSpeed);
	float move = m_curSpeed * fTimeDelta;
	if (m_curSpeed > 0.f)
	{

		m_transformCom->Go_Forward_Distanace((move));

	}
	if (m_curSpeed < 0.f)
	{

		m_transformCom->Go_Backward_Distanace(abs(move));

	}


	float turnInput = 0.f;
	if (dinput->KeyPress(DIK_D))
		turnInput += 1.f;
	if (dinput->KeyPress(DIK_A))
		turnInput -= 1.f;

	if (turnInput != 0.f)
	{
		m_curTurnSpeed += turnInput * m_turnAcceleration * fTimeDelta;
		m_curTurnSpeed = clamp(m_curTurnSpeed, -m_maxTurnSpeed, m_maxTurnSpeed);
	}
	else
	{
		const float turnDeceleration = m_turnDeceleration * fTimeDelta;
		if (m_curTurnSpeed > 0.f)
			m_curTurnSpeed = max(0.f, m_curTurnSpeed - turnDeceleration);
		else if (m_curTurnSpeed < 0.f)
			m_curTurnSpeed = min(0.f, m_curTurnSpeed + turnDeceleration);
	}

	if (abs(m_curTurnSpeed) > 0.0001f)
	{
		m_transformCom->Set_RadianSpeed(abs(m_curTurnSpeed));
		m_transformCom->Turn(XMLoadFloat4(&upDir), m_curTurnSpeed > 0.f ? fTimeDelta : -fTimeDelta);
	}

return 	ETOI(EPlayerState::SEA);
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
