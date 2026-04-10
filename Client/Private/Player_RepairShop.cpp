#include "Player_RepairShop.h"

#include <UI.h>

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
	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;
	pLerp->vTargetPos = _float3(13.9f, 3.8f, 9.29f);
	pLerp->vTargetRot = _float3(5.f, -121.f, 0.f);
	pLerp->fDuration = 2.0f;
	pLerp->fFov = 30.f;
	pLerp->OnComplete = [this]() {
			auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	m_Village->UI_Active();
	};
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);



	CPlayerState::Enter();
}

void CPlayer_RepairShop::Exit()
{
	auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	m_Village->UI_InActive();
	CPlayerState::Exit();
}

HRESULT CPlayer_RepairShop::Init_State()
{



	return CPlayerState::Init_State();
}

int CPlayer_RepairShop::Update_State(const _float& timeDelta)
{

	if(m_Input_Manager->KeyDown(DIK_X))
	{

		return  ETOI(PLAYERSTATE::SEA);
	}


	return ETOI(PLAYERSTATE::VILLAGE);
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
