#include "Player_Village.h"

#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"
#include "Camera_Play.h"

CPlayer_Village::CPlayer_Village(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_Village::~CPlayer_Village()
{
}

void CPlayer_Village::Enter()
{

	float duration = 2.f;
	if(m_Flag !=  true)
	{
		// 시간 6시로 
		 m_pGameInstance.lock()->Set_TOD01(0.25);

		 Evt_ChangeCam event = {};
		 auto pLerp = make_shared<CAM_LERP_DESC>();
		 pLerp->eMode = CAM_MODE::LERP;
		 pLerp->vTargetPos = _float3(13.9f, 3.8f, 9.29f);
		 pLerp->vTargetRot = _float3(5.f, -121.f, 0.f);
		 pLerp->fDuration = 0.f;
		 pLerp->fFov = 30.f;
		 pLerp->OnComplete = [this]() {

			 Evt_Cam_Arrived e = {};
			 e.playerstate = E_PLAYERSTATE::VILLAGE;
			 CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
			 //		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
			 //m_Village->UI_Active();
			 };
		 event.commands.push_back(pLerp);

		 auto pStop = make_shared<CAM_DESC>();
		 pStop->eMode = CAM_MODE::STOP;
		 event.commands.push_back(pStop);
	
		return;
	
	}
	
	

	CDialogueDB::GetInstance()->Set_PendingDialogue(m_VecDialogue);


	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;
	pLerp->vTargetPos = _float3(13.9f, 3.8f, 9.29f);
	pLerp->vTargetRot = _float3(5.f, -121.f, 0.f);
	pLerp->fDuration = duration;
	pLerp->fFov = 30.f;
	pLerp->OnComplete = [this]() {

		Evt_Cam_Arrived e = {};
		e.playerstate = E_PLAYERSTATE::VILLAGE;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
	//		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	//m_Village->UI_Active();
	};
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);


	
	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

	eNextState = E_PLAYERSTATE::VILLAGE;

	CPlayerState::Enter();
}


void CPlayer_Village::Exit()
{
	//auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	//m_Village->UI_InActive();


	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

	m_VecDialogue.clear();
	CPlayerState::Exit();
}

HRESULT CPlayer_Village::Init_State()
{
	//m_VecDialogue.push_back("Start");
	//m_VecDialogue.push_back("Mayer_start");


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeState>([this](const Evt_ChangeState& e)
	{
			eNextState = e.playerstate;
	});


	return CPlayerState::Init_State();
}

int CPlayer_Village::Update_State(const _float& timeDelta)
{
	if (m_Flag != true)
	{
		m_Acc += timeDelta;




		if(m_Acc > 4.f)
		{

			CDialogueDB::GetInstance()->Set_PendingDialogue(m_VecDialogue);


			Evt_ChangeCam event = {};
			auto pLerp = make_shared<CAM_LERP_DESC>();
			pLerp->eMode = CAM_MODE::LERP;
			pLerp->vTargetPos = _float3(13.9f, 3.8f, 9.29f);
			pLerp->vTargetRot = _float3(5.f, -121.f, 0.f);
			pLerp->fDuration = 5.f;
			pLerp->fFov = 30.f;
			pLerp->OnComplete = [this]() {

				Evt_Cam_Arrived e = {};
				e.playerstate = E_PLAYERSTATE::VILLAGE;
				CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
				//		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
				//m_Village->UI_Active();
				};
			event.commands.push_back(pLerp);

			auto pStop = make_shared<CAM_DESC>();
			pStop->eMode = CAM_MODE::STOP;
			event.commands.push_back(pStop);



			CGameInstance::GetInstance()->Get_EventBus()->Publish(event);




		m_Flag = true;
		}
	}


	if(m_Input_Manager->KeyDown(DIK_X))
	{

		return  ETOI(E_PLAYERSTATE::SEA);
	}


	return ETOI(eNextState);
}

void CPlayer_Village::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_Village::Render_State()
{
	
}

shared_ptr<CPlayer_Village> CPlayer_Village::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_Village> pInstance(new CPlayer_Village(owner,pStateMachine), [](CPlayer_Village* p) {p->Free(); delete(p); });


	return pInstance;
}

void CPlayer_Village::Free()
{
	//m_VecDialogue.clear();

	CPlayerState::Free();
}
