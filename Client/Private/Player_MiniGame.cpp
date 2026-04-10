#include "Player_MiniGame.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
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

	auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
	dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(TAB::INVEN), TAB::INVEN);

	auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
	m_HoldItem->UI_Active();

	auto MiniGame = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"MiniGame");
	auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
	ui->UI_PanelActive(CUI_MiniGame::BASIC_CIRCLE, 1002);

	
	CPlayerState::Enter();
}

void CPlayer_MiniGame::Exit()
{

	auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
	dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_InActive();

	auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
	m_HoldItem->UI_InActive();

	auto MiniGame = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"MiniGame");
	auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
	ui->UI_InActive();


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
	return ETOI(PLAYERSTATE::SEA);
	}
		return  ETOI(PLAYERSTATE::FISHING);
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
