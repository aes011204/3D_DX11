#include "PlayerStateMachine.h"

#include "PlayerBoat.h"
#include "State.h"
#include "PlayerState.h"
#include "Player_FishShop.h"
#include "Player_Interact.h"
#include "Player_MiniGame.h"
#include "Player_RepairShop.h"
#include "Player_Sea.h"
#include "Player_Storage.h"
#include "Player_Village.h"

CPlayerStateMachine::CPlayerStateMachine(weak_ptr<CPlayerBoat> owner)
	: m_owner(owner)
{
}

CPlayerStateMachine::~CPlayerStateMachine()
{
}

HRESULT CPlayerStateMachine::Init_StateMachine()
{
    Init_PlayerStates();

    m_curState = m_vecState[ETOI(EPlayerState::VILLAGE)];

    m_curState->Enter();

    return S_OK;
}

_int CPlayerStateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_curState)
    {
        _uint nextState = m_curState->Update_State(timeDelta);

        if (nextState == ETOI(EPlayerState::END))
        {
            return nextState;
        }

        if (nextState < ETOI(EPlayerState::END) && nextState != m_curStateKey)
        {
            Change_State(nextState);
        }
    }

    return m_curStateKey;

}

void CPlayerStateMachine::LateUpdate_StateMachine(const _float& timeDelta)
{
    m_curState->LateUpdate_State(timeDelta);
}

void CPlayerStateMachine::Change_State(_uint changeStateKey)
{
    if (m_curStateKey == changeStateKey)
        return; // 이전이랑 같은 상황

    if (m_vecState[changeStateKey] == nullptr || changeStateKey >= ETOI(EPlayerState::END))
        return; // 인덱스 범위 확인

    // 이전 상태 나가기
    if (m_curState)
        m_curState->Exit();

    // 이전 상태 등록
    //m_pPrevState = m_CurState;
    m_prevStateKey = m_curStateKey;
    m_curStateKey = changeStateKey;

    // 새 상태로 가져오기
    m_curState = m_vecState[changeStateKey];
    m_curState->Enter();
}

void CPlayerStateMachine::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (m_curState)
        m_curState->OnBeginOverlap(self, other);
}

void CPlayerStateMachine::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (m_curState)
        m_curState->OnEndOverlap(self, other);
}

void CPlayerStateMachine::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (m_curState)
        m_curState->OnStayOverlap(self, other);
}

_int CPlayerStateMachine::GetCurStateNum()
{
    return m_curState->Get_StateKey();
}

shared_ptr<CPlayerState> CPlayerStateMachine::Get_State(_uint stateKey)
{
    if (stateKey < m_vecState.size())
        return static_pointer_cast<CPlayerState>(m_vecState[stateKey]);

    return nullptr;
}

HRESULT CPlayerStateMachine::Init_PlayerStates()
{
    m_vecState.resize(ETOI(EPlayerState::END));
   shared_ptr<CPlayerState> state = nullptr;

    if (nullptr == (state = CPlayer_Sea::Create(m_owner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(EPlayerState::SEA)] = state;
    state->Init_State();

    if (nullptr == (state = CPlayer_MiniGame::Create(m_owner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(EPlayerState::FISHING)] = state;
    state->Init_State();

    if (nullptr == (state = CPlayer_Village::Create(m_owner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(EPlayerState::VILLAGE)] = state;
    state->Init_State();




    if (nullptr == (state = CPlayer_Storage::Create(m_owner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(EPlayerState::STORAGE)] = state;
    state->Init_State();

        if (nullptr == (state = CPlayer_RepairShop::Create(m_owner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
            return E_FAIL;
    m_vecState[ETOI(EPlayerState::REPAIR_SHOP)] = state;
    state->Init_State();;

        if (nullptr == (state = CPlayer_FishShop::Create(m_owner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
            return E_FAIL;
    m_vecState[ETOI(EPlayerState::FISH_SHOP)] = state;
    state->Init_State();

    if (nullptr == (state = CPlayer_Interact::Create(m_owner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(EPlayerState::INTERACT)] = state;
    state->Init_State();

    return S_OK;
}

shared_ptr<CPlayerStateMachine> CPlayerStateMachine::Create(shared_ptr<CPlayerBoat> owner)
{
    shared_ptr<CPlayerStateMachine> instance(new CPlayerStateMachine(owner), [](CPlayerStateMachine* p) {p->Free(); delete(p); });

    if (FAILED(instance->Init_StateMachine()))
    {
        MSG_BOX("Failed to Created : CUIPanel");

    }
    return instance;
}

void CPlayerStateMachine::Free()
{
	CStateMachine::Free();
}
