#include "PlayerState.h"

#include "PlayerBoat.h"
#include "Camera_Play.h"

CPlayerState::CPlayerState(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> stateMachine)
	: m_owner(owner), m_stateMachine(stateMachine), m_gameInstance(CGameInstance::GetInstance())
{
}

CPlayerState::~CPlayerState()
{

}

void CPlayerState::Enter()
{
	CState::Enter();


}

void CPlayerState::Exit()
{
	CState::Exit();

	m_dialogueList.clear();

}

HRESULT CPlayerState::Init_State()
{
	Find_Player();
	return S_OK;
}

void CPlayerState::Find_Player()
{

	auto transform =dynamic_pointer_cast<CTransform>( m_owner.lock()->Get_Component(g_strTransformTag));
	if (transform == nullptr)
		MSG_BOX("failed to Get Com in StateMachine");
	m_ownerTransformCom = transform;

	m_inputManager = CGameInstance::GetInstance()->Get_DInput_Manger();


}
