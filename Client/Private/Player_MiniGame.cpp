#include "Player_MiniGame.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "EventBus.h"
#include "Fish.h"
#include "PlayerStateMachine.h"
#include "UI_Item.h"
#include "UI_MiniGame.h"
#include "UI_TabContainer.h"

CPlayer_MiniGame::CPlayer_MiniGame(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> stateMachine)
	: CPlayerState(owner, stateMachine)
{
}

CPlayer_MiniGame::~CPlayer_MiniGame()
{
}

void CPlayer_MiniGame::Enter()
{


	Evt_ChangeCam event = {};
	auto lerp = make_shared<CamLerpDesc>();
	lerp->mode = ECamMode::LERP;

	_vector forward = m_ownerTransformCom.lock()->Get_State(ETransformState::LOOK);
	XMStoreFloat3(&lerp->targetPos, m_ownerTransformCom.lock()->Get_Position()
		- XMVector3Normalize(forward) * 3.f   // 살짝 뒤로
		+ XMVectorSet(0.f, 20.f, 0.f, 0.f));   // 위);
	lerp->target = m_owner;
	lerp->duration = 1.5f;
	lerp->OnComplete = [this]() {
		Evt_CamArrived e = {};
		e.playerstate = EPlayerState::FISHING;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
		
		};
	event.commands.push_back(lerp);


	auto stop = make_shared<CamDesc>();
	stop->mode = ECamMode::STOP;
	event.commands.push_back(stop);

	m_gameInstance.lock()->Get_EventBus()->Publish(event);



	CPlayerState::Enter();
}

void CPlayer_MiniGame::Exit()
{

	/*auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
	dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_InActive();

	auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
	m_HoldItem->UI_InActive();

	auto MiniGame = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"MiniGame");
	auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
	ui->UI_InActive();*/

	if(m_stateMachine.lock()->Get_TargetShared() != nullptr)
	{
		auto fish = dynamic_pointer_cast<CFish>(m_stateMachine.lock()->Get_TargetShared());
		if(fish->GetCurFishCount() == 0)
		{
			fish->Mark_Destroy();
		}
	}

	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

	CPlayerState::Exit();
}

HRESULT CPlayer_MiniGame::Init_State()
{


	

	return CPlayerState::Init_State();
}

int CPlayer_MiniGame::Update_State(const _float& timeDelta)
{
	
	if(m_inputManager->KeyDown(DIK_X))
	{
	return ETOI(EPlayerState::SEA);
	}
		return  ETOI(EPlayerState::FISHING);
}

void CPlayer_MiniGame::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_MiniGame::Render_State()
{
	
}

shared_ptr<CPlayer_MiniGame> CPlayer_MiniGame::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> stateMachine)
{
	shared_ptr<CPlayer_MiniGame> instance(new CPlayer_MiniGame(owner,stateMachine), [](CPlayer_MiniGame* p) {p->Free(); delete(p); });

	//if (FAILED(pInstance->Init_State()))
	//{
	//	MSG_BOX("Failed to Created : CUIPanel");
	//
	//}
	return instance;
}

void CPlayer_MiniGame::Free()
{
	CPlayerState::Free();
}
