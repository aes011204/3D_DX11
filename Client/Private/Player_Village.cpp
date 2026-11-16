#include "Player_Village.h"

#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"
#include "Camera_Play.h"
#include "UI_Controller.h"

CPlayer_Village::CPlayer_Village(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> stateMachine)
	: CPlayerState(owner, stateMachine)
{
}

CPlayer_Village::~CPlayer_Village()
{
}

void CPlayer_Village::Enter()
{
	if (m_owner.lock()->Get_WasDead())
	{
		m_dialogueList.push_back("Mayer_End");

	}
	CGameInstance::GetInstance()->Set_TimeScale(1.f);


	float duration = 2.f;
	if(m_isFlag !=  true)
	{
		// 시간 6시로

		 m_gameInstance.lock()->Set_TOD01(0.25);

		 //Evt_ChangeCam event = {};
		 //auto pLerp = make_shared<CAM_LERP_DESC>();
		 //pLerp->eMode = CAM_MODE::LERP;
		 //pLerp->vTargetPos = _float3(13.9f, 3.8f, 9.29f);
		 //pLerp->vTargetRot = _float3(5.f, -121.f, 0.f);
		 //pLerp->fDuration = 0.f;
		 //pLerp->fFov = 30.f;
		 //pLerp->OnComplete = [this]() {

			// Evt_Cam_Arrived e = {};
			// e.playerstate = E_PLAYERSTATE::VILLAGE;
			// CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
			// //		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
			// //m_Village->UI_Active();
			// };
		 //event.commands.push_back(pLerp);

		 //auto pStop = make_shared<CAM_DESC>();
		 //pStop->eMode = CAM_MODE::STOP;
		 //event.commands.push_back(pStop);

		 //CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

		 //eNextState = E_PLAYERSTATE::VILLAGE;
		 //m_Flag = true;
		return;
	
	}
	
	
	CDialogueDB::GetInstance()->Set_PendingDialogue(m_dialogueList);



	Evt_ChangeCam event = {};
	auto lerp = make_shared<CamLerpDesc>();
	lerp->mode = ECamMode::LERP;
	lerp->targetPos = _float3(13.9f, 3.8f, 9.29f);
	lerp->targetRot = _float3(5.f, -121.f, 0.f);
	lerp->duration = duration;
	lerp->FOV = 30.f;
	lerp->OnComplete = [this]() {

		Evt_CamArrived e = {};
		e.playerstate = EPlayerState::VILLAGE;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
	//		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	//m_Village->UI_Active();
	};
	event.commands.push_back(lerp);

	auto stop = make_shared<CamDesc>();
	stop->mode = ECamMode::STOP;
	event.commands.push_back(stop);

	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

	m_nextState = EPlayerState::VILLAGE;
	//m_pGameInstance.lock()->Play_Loop(L"Waves_Ambience_1");

	CPlayerState::Enter();
}


void CPlayer_Village::Exit()
{
	//auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	//m_Village->UI_InActive();


	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

	m_dialogueList.clear();
	CDialogueDB::GetInstance()->Set_PendingDialogue(m_dialogueList);

	//m_pGameInstance.lock()->Stop(L"GM_BGM_Theme");

	CPlayerState::Exit();
}

HRESULT CPlayer_Village::Init_State()
{




	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeState>([this](const Evt_ChangeState& e)
		{
			m_nextState = e.playerstate;
		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe < Evt_VillageBtn>([this](const Evt_VillageBtn& e)
	{
		if(e.type ==0)
		{
			m_nextState = EPlayerState::SEA;
		}
		else if(e.type == 1)
		{
			// 잠자기
			CUI_Controller::GetInstance()->ActiveTime(999, true);
			m_isSleep = true;
		}
		else if (e.type == 2)
		{
			//
		}
	});


	return CPlayerState::Init_State();
}

int CPlayer_Village::Update_State(const _float& timeDelta)
{
	if (m_isFlag != true)
	{
		m_acc += timeDelta;




		if(m_acc > 4.f)
		{

			m_dialogueList.push_back("Start");
			m_dialogueList.push_back("Mayer_start");
			CDialogueDB::GetInstance()->Set_PendingDialogue(m_dialogueList);


			Evt_ChangeCam event = {};
			auto lerp = make_shared<CamLerpDesc>();
			lerp->mode = ECamMode::LERP;
			lerp->targetPos = _float3(13.9f, 3.8f, 9.29f);
			lerp->targetRot = _float3(5.f, -121.f, 0.f);
			lerp->duration = 5.f;
			lerp->FOV = 30.f;
			lerp->OnComplete = [this]() {

				Evt_CamArrived e = {};
				e.playerstate = EPlayerState::VILLAGE;
				CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
				//		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
				//m_Village->UI_Active();
				};
			event.commands.push_back(lerp);

			auto stop = make_shared<CamDesc>();
			stop->mode = ECamMode::STOP;
			event.commands.push_back(stop);



			CGameInstance::GetInstance()->Get_EventBus()->Publish(event);




		m_isFlag = true;
		}
	}


	if(m_inputManager->KeyDown(DIK_X))
	{
		if(m_isSleep == true)
		{
			CUI_Controller::GetInstance()->InActiveTime();
			m_gameInstance.lock()->Play_Once(L"Hold_Complete");
			return ETOI(m_nextState);
		}

		return  ETOI(EPlayerState::SEA);
	}


	return ETOI(m_nextState);
}

void CPlayer_Village::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_Village::Render_State()
{
	
}

shared_ptr<CPlayer_Village> CPlayer_Village::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> stateMachine)
{
	shared_ptr<CPlayer_Village> instance(new CPlayer_Village(owner,stateMachine), [](CPlayer_Village* p) {p->Free(); delete(p); });


	return instance;
}

void CPlayer_Village::Free()
{
	//m_VecDialogue.clear();

	CPlayerState::Free();
}
