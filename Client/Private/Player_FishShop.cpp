#include "Player_FishShop.h"

#include <UI.h>

#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"
#include "UI_NPC.h"

CPlayer_FishShop::CPlayer_FishShop(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_FishShop::~CPlayer_FishShop()
{
}

void CPlayer_FishShop::Enter()
{
	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;
	pLerp->vTargetPos = _float3(0.688f, 2.814f, 1.268f);
	pLerp->vTargetRot = _float3(21.49f, -50.13f, 0.f);
	pLerp->fDuration = 1.0f;
	//pLerp->fFov = 30.f;
	pLerp->OnComplete = [this]() {
			auto m_NPC = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC");
			auto ui = dynamic_pointer_cast<CUI_NPC>(m_NPC);
			ui->UI_NPCActive(NPC::MAYOR, true, true, "FishStore_First");
	};
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);



	CPlayerState::Enter();
}

void CPlayer_FishShop::Exit()
{
	auto m_NPC = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC");
	m_NPC->UI_InActive();
	CPlayerState::Exit();
}

HRESULT CPlayer_FishShop::Init_State()
{



	return CPlayerState::Init_State();
}

int CPlayer_FishShop::Update_State(const _float& timeDelta)
{

	if(m_Input_Manager->KeyDown(DIK_X))
	{

		return  ETOI(PLAYERSTATE::VILLAGE);
	}


	return ETOI(PLAYERSTATE::VILLAGE);
}

void CPlayer_FishShop::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_FishShop::Render_State()
{
	
}

shared_ptr<CPlayer_FishShop> CPlayer_FishShop::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_FishShop> pInstance(new CPlayer_FishShop(owner,pStateMachine), [](CPlayer_FishShop* p) {p->Free(); delete(p); });


	return pInstance;
}

void CPlayer_FishShop::Free()
{
	CPlayerState::Free();
}
