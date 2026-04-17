#include "Player_Storage.h"
#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"

CPlayer_Storage::CPlayer_Storage(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_Storage::~CPlayer_Storage()
{
}

void CPlayer_Storage::Enter()
{
	CDialogueDB::GetInstance()->Set_PendingDialogue(list<string> {});

	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;
	pLerp->vTargetPos = _float3(4.516f, 3.944f, 0.87f);
	pLerp->vTargetRot = _float3(33.84f, -55.60f, 0.f);
	pLerp->fDuration = 2.0f;
	pLerp->fFov = 30.f;
	pLerp->OnComplete = [this]() {
		Evt_Cam_Arrived e = {};
		e.playerstate = E_PLAYERSTATE::STORAGE;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
	};
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

	eNextState = E_PLAYERSTATE::STORAGE;


	Evt_OpenInventory e;
	e.inven = m_Owner.lock()->Get_StorageCom();
	m_pGameInstance.lock()->Get_EventBus()->Publish(e);


	CPlayerState::Enter();
}

void CPlayer_Storage::Exit()
{

	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);



	Evt_CloseInventory close;
	m_pGameInstance.lock()->Get_EventBus()->Publish(close);



	CPlayerState::Exit();
}

HRESULT CPlayer_Storage::Init_State()
{



	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeState>([this](const Evt_ChangeState& e)
		{
			eNextState = e.playerstate;
		});


	return CPlayerState::Init_State();
}

int CPlayer_Storage::Update_State(const _float& timeDelta)
{

	if(m_Input_Manager->KeyDown(DIK_X))
	{

		return  ETOI(E_PLAYERSTATE::VILLAGE);
	}


	return ETOI(eNextState);
}


void CPlayer_Storage::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_Storage::Render_State()
{
	
}

shared_ptr<CPlayer_Storage> CPlayer_Storage::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_Storage> pInstance(new CPlayer_Storage(owner,pStateMachine), [](CPlayer_Storage* p) {p->Free(); delete(p); });


	return pInstance;
}

void CPlayer_Storage::Free()
{
	CPlayerState::Free();
}
