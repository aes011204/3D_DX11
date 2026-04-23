#include "Player_RepairShop.h"

#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"

CPlayer_RepairShop::CPlayer_RepairShop(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_RepairShop::~CPlayer_RepairShop()
{
}

void CPlayer_RepairShop::Enter()
{
	CDialogueDB::GetInstance()->Set_PendingDialogue(m_VecDialogue);


	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;
	pLerp->vTargetPos = _float3(3.14f, 3.429f, 5.734f);
	pLerp->vTargetRot = _float3(5.58f, -145.63f, 0.f);
	pLerp->fDuration = 2.0f;
	pLerp->fFov = 30.f;
	pLerp->OnComplete = [this]() {
		Evt_Cam_Arrived e = {};
		e.playerstate = E_PLAYERSTATE::REPAIR_SHOP;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
	};
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

	eNextState = E_PLAYERSTATE::REPAIR_SHOP;

	CPlayerState::Enter();
}

void CPlayer_RepairShop::Exit()
{
	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

	CPlayerState::Exit();
}

HRESULT CPlayer_RepairShop::Init_State()
{

	m_VecDialogue.push_back("Repair_First");

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeState>([this](const Evt_ChangeState& e)
		{
			eNextState = e.playerstate;
		});





	return CPlayerState::Init_State();
}

int CPlayer_RepairShop::Update_State(const _float& timeDelta)
{

	if(m_Input_Manager->KeyDown(DIK_X))
	{

		return  ETOI(E_PLAYERSTATE::VILLAGE);
	}


	return ETOI(eNextState);
}

void CPlayer_RepairShop::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_RepairShop::Render_State()
{
	
}

shared_ptr<CPlayer_RepairShop> CPlayer_RepairShop::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_RepairShop> pInstance(new CPlayer_RepairShop(owner,pStateMachine), [](CPlayer_RepairShop* p) {p->Free(); delete(p); });


	return pInstance;
}

void CPlayer_RepairShop::Free()
{
	CPlayerState::Free();
}
