#include "Player_Village.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"

CPlayer_Village::CPlayer_Village(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_Village::~CPlayer_Village()
{
}

void CPlayer_Village::Enter()
{
	CPlayerState::Enter();
}

void CPlayer_Village::Exit()
{
	CPlayerState::Exit();
}

HRESULT CPlayer_Village::Init_State()
{



	return CPlayerState::Init_State();
}

int CPlayer_Village::Update_State(const _float& timeDelta)
{
	
	return ETOI(PLAYERSTATE::VILLAGE);
}

void CPlayer_Village::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_Village::Render_State()
{
	
}

shared_ptr<CPlayer_Village> CPlayer_Village::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_Village> pInstance(new CPlayer_Village(owner,pStateMachine), [](CPlayer_Village* p) {p->Free(); delete(p); });


	return pInstance;
}

void CPlayer_Village::Free()
{
	CPlayerState::Free();
}
