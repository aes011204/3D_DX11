#include "Player_MiniGame.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"

CPlayer_MiniGame::CPlayer_MiniGame(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_MiniGame::~CPlayer_MiniGame()
{
}

void CPlayer_MiniGame::Enter()
{
	CPlayerState::Enter();
}

void CPlayer_MiniGame::Exit()
{
	CPlayerState::Exit();
}

HRESULT CPlayer_MiniGame::Init_State()
{


	

	return CPlayerState::Init_State();
}

int CPlayer_MiniGame::Update_State(const _float& timeDelta)
{
	

	return ETOI(PLAYERSTATE::SEA);
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
