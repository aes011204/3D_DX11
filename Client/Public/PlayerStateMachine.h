#pragma once
#include "StateMachine.h"
#include "Client_Enum.h"
#include "Client_Define.h"


NS_BEGIN(Engine)

class CCollider;
	class CGameObject;
class CState;
NS_END

NS_BEGIN(Client)
class CPlayerStateMachine : public CStateMachine
{
protected:
    explicit CPlayerStateMachine(weak_ptr<class CPlayerBoat> owner);
public:
    virtual ~CPlayerStateMachine();

public:
    virtual HRESULT Init_StateMachine();
    _int    Update_StateMachine(const _float& timeDelta);
    void    LateUpdate_StateMachine(const _float& timeDelta);


    virtual void Change_State(_uint changeStateKey);

    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other);
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) ;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) ;





	_int GetCurStateNum();

    weak_ptr<CState> Get_CurState() { return m_CurState; }
    _uint Get_PrevStateKey() { return  m_PrevStateKey; }

    shared_ptr<class CPlayerState> Get_State(_uint stateKey);

private:
   weak_ptr<class CPlayerBoat> m_pOwner;

protected:
    HRESULT Init_PlayerStates();

public:
    static shared_ptr<CPlayerStateMachine> Create(shared_ptr<CPlayerBoat> owner);

protected:
    virtual void Free();
};

NS_END