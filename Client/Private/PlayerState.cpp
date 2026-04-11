#include "PlayerState.h"

#include "PlayerBoat.h"
#include "Camera_Play.h"

CPlayerState::CPlayerState(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: m_Owner(owner), m_pStateMachine(pStateMachine), m_pGameInstance(CGameInstance::GetInstance())
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

	m_VecDialogue.clear();

}

HRESULT CPlayerState::Init_State()
{
	Find_Player();
	return S_OK;
}

void CPlayerState::Find_Player()
{

	auto transform =dynamic_pointer_cast<CTransform>( m_Owner.lock()->Get_Component(g_strTransformTag));
	if (transform == nullptr)
		MSG_BOX("failed to Get Com in StateMachine");
	m_pOwnerTransformCom = transform;

	m_Input_Manager = CGameInstance::GetInstance()->Get_DInput_Manger();


}
