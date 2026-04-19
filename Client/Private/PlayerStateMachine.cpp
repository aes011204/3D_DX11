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
	: m_pOwner(owner)
{
}

CPlayerStateMachine::~CPlayerStateMachine()
{
}

HRESULT CPlayerStateMachine::Init_StateMachine()
{
    Init_PlayerStates();

    m_CurState = m_vecState[ETOI(E_PLAYERSTATE::SEA)];

    m_CurState->Enter();

    return S_OK;
}

_int CPlayerStateMachine::Update_StateMachine(const _float& timeDelta)
{
    if (m_CurState)
    {
        _uint nextState = m_CurState->Update_State(timeDelta);

        if (nextState == ETOI(E_PLAYERSTATE::END))
        {
            return nextState;
        }

        if (nextState < ETOI(E_PLAYERSTATE::END) && nextState != m_CurStateKey)
        {
            Change_State(nextState);
        }
    }

    return m_CurStateKey;

}

void CPlayerStateMachine::LateUpdate_StateMachine(const _float& timeDelta)
{
    m_CurState->LateUpdate_State(timeDelta);
}

void CPlayerStateMachine::Change_State(_uint changeStateKey)
{
    if (m_CurStateKey == changeStateKey)
        return; // 이전이랑 같은 상황

    if (m_vecState[changeStateKey] == nullptr || changeStateKey >= ETOI(E_PLAYERSTATE::END))
        return; // 인덱스 범위 확인

    // 이전 상태 나가기
    if (m_CurState)
        m_CurState->Exit();

    // 이전 상태 등록
    //m_pPrevState = m_CurState;
    m_PrevStateKey = m_CurStateKey;
    m_CurStateKey = changeStateKey;

    // 새 상태로 가져오기
    m_CurState = m_vecState[changeStateKey];
    m_CurState->Enter();
}

void CPlayerStateMachine::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (m_CurState)
        m_CurState->OnBeginOverlap(self, other);
}

void CPlayerStateMachine::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (m_CurState)
        m_CurState->OnEndOverlap(self, other);
}

void CPlayerStateMachine::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (m_CurState)
        m_CurState->OnStayOverlap(self, other);
}

_int CPlayerStateMachine::GetCurStateNum()
{
    return m_CurState->Get_StateKey();
}

shared_ptr<CPlayerState> CPlayerStateMachine::Get_State(_uint stateKey)
{
    if (stateKey < m_vecState.size())
        return static_pointer_cast<CPlayerState>(m_vecState[stateKey]);

    return nullptr;
}

HRESULT CPlayerStateMachine::Init_PlayerStates()
{
    m_vecState.resize(ETOI(E_PLAYERSTATE::END));
   shared_ptr<CPlayerState> pState = nullptr;

    if (nullptr == (pState = CPlayer_Sea::Create(m_pOwner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(E_PLAYERSTATE::SEA)] = pState;
    pState->Init_State();

    if (nullptr == (pState = CPlayer_MiniGame::Create(m_pOwner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(E_PLAYERSTATE::FISHING)] = pState;
    pState->Init_State();

    if (nullptr == (pState = CPlayer_Village::Create(m_pOwner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(E_PLAYERSTATE::VILLAGE)] = pState;
    pState->Init_State();




    if (nullptr == (pState = CPlayer_Storage::Create(m_pOwner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(E_PLAYERSTATE::STORAGE)] = pState;
    pState->Init_State();

        if (nullptr == (pState = CPlayer_RepairShop::Create(m_pOwner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
            return E_FAIL;
    m_vecState[ETOI(E_PLAYERSTATE::REPAIR_SHOP)] = pState;
    pState->Init_State();;

        if (nullptr == (pState = CPlayer_FishShop::Create(m_pOwner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
            return E_FAIL;
    m_vecState[ETOI(E_PLAYERSTATE::FISH_SHOP)] = pState;
    pState->Init_State();

    if (nullptr == (pState = CPlayer_Interact::Create(m_pOwner.lock(), dynamic_pointer_cast<CPlayerStateMachine>(shared_from_this()))))
        return E_FAIL;
    m_vecState[ETOI(E_PLAYERSTATE::INTERACT)] = pState;
    pState->Init_State();

    return S_OK;
}

shared_ptr<CPlayerStateMachine> CPlayerStateMachine::Create(shared_ptr<CPlayerBoat> owner)
{
    shared_ptr<CPlayerStateMachine> pInstance(new CPlayerStateMachine(owner), [](CPlayerStateMachine* p) {p->Free(); delete(p); });

    if (FAILED(pInstance->Init_StateMachine()))
    {
        MSG_BOX("Failed to Created : CUIPanel");

    }
    return pInstance;
}

void CPlayerStateMachine::Free()
{
	CStateMachine::Free();
}
