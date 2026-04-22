#include "Player_MiniGame.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "EventBus.h"
#include "Fish.h"
#include "PlayerStateMachine.h"
#include "UI_Item.h"
#include "UI_MiniGame.h"
#include "UI_TabContainer.h"

CPlayer_MiniGame::CPlayer_MiniGame(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_MiniGame::~CPlayer_MiniGame()
{
}

void CPlayer_MiniGame::Enter()
{


	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;

	_vector forward = m_pOwnerTransformCom.lock()->Get_State(STATE::LOOK);
	XMStoreFloat3(&pLerp->vTargetPos, m_pOwnerTransformCom.lock()->Get_Position()
		- XMVector3Normalize(forward) * 3.f   // »ìÂ¦ µÚ·Î
		+ XMVectorSet(0.f, 20.f, 0.f, 0.f));   // À§);
	pLerp->m_Target = m_Owner;
	pLerp->fDuration = 1.5f;
	pLerp->OnComplete = [this]() {
		Evt_Cam_Arrived e = {};
		e.playerstate = E_PLAYERSTATE::FISHING;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
		
		};
	event.commands.push_back(pLerp);


	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);

	m_pGameInstance.lock()->Get_EventBus()->Publish(event);



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

	if(m_pStateMachine.lock()->Get_TargetShared() != nullptr)
	{
		auto fish = dynamic_pointer_cast<CFish>(m_pStateMachine.lock()->Get_TargetShared());
		if(fish->GetFishCount() == 0)
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
	
	if(m_Input_Manager->KeyDown(DIK_X))
	{
	return ETOI(E_PLAYERSTATE::SEA);
	}
		return  ETOI(E_PLAYERSTATE::FISHING);
}

void CPlayer_MiniGame::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_MiniGame::Render_State()
{
	
}

shared_ptr<CPlayer_MiniGame> CPlayer_MiniGame::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_MiniGame> pInstance(new CPlayer_MiniGame(owner,pStateMachine), [](CPlayer_MiniGame* p) {p->Free(); delete(p); });

	//if (FAILED(pInstance->Init_State()))
	//{
	//	MSG_BOX("Failed to Created : CUIPanel");
	//
	//}
	return pInstance;
}

void CPlayer_MiniGame::Free()
{
	CPlayerState::Free();
}
