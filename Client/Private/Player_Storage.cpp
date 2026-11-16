#include "Player_Storage.h"
#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"

CPlayer_Storage::CPlayer_Storage(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> stateMachine)
	: CPlayerState(owner, stateMachine)
{
}

CPlayer_Storage::~CPlayer_Storage()
{
}

void CPlayer_Storage::Enter()
{
	CDialogueDB::GetInstance()->Set_PendingDialogue(list<string> {});

	Evt_ChangeCam event = {};
	auto lerp = make_shared<CamLerpDesc>();
	lerp->mode = ECamMode::LERP;
	lerp->targetPos = _float3(3.016f, 2.944f, 0.87f);
	lerp->targetRot = _float3(33.84f, -55.60f, 0.f);
	lerp->duration = 2.0f;
	lerp->FOV = 30.f;
	lerp->OnComplete = [this]() {
		Evt_CamArrived e = {};
		e.playerstate = EPlayerState::STORAGE;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
	};
	event.commands.push_back(lerp);

	auto stop = make_shared<CamDesc>();
	stop->mode = ECamMode::STOP;
	event.commands.push_back(stop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

	m_nextState = EPlayerState::STORAGE;


	/*Evt_OpenInventory e;
	e.inven = m_Owner.lock()->Get_StorageCom();
	m_pGameInstance.lock()->Get_EventBus()->Publish(e);*/


	CPlayerState::Enter();
}

void CPlayer_Storage::Exit()
{

	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);



	Evt_CloseInventory close;
	m_gameInstance.lock()->Get_EventBus()->Publish(close);



	CPlayerState::Exit();
}

HRESULT CPlayer_Storage::Init_State()
{



	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeState>([this](const Evt_ChangeState& e)
		{
			m_nextState = e.playerstate;
		});


	return CPlayerState::Init_State();
}

int CPlayer_Storage::Update_State(const _float& timeDelta)
{

	if(m_inputManager->KeyDown(DIK_X))
	{

		return  ETOI(EPlayerState::VILLAGE);
	}


	return ETOI(m_nextState);
}


void CPlayer_Storage::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_Storage::Render_State()
{
	
}

shared_ptr<CPlayer_Storage> CPlayer_Storage::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> stateMachine)
{
	shared_ptr<CPlayer_Storage> instance(new CPlayer_Storage(owner,stateMachine), [](CPlayer_Storage* p) {p->Free(); delete(p); });


	return instance;
}

void CPlayer_Storage::Free()
{
	CPlayerState::Free();
}
