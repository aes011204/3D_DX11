#pragma once
#include "PlayerState.h"


NS_BEGIN(Client)

	class CPlayerBoat;
	class CPlayerStateMachine;
class CPlayer_Interact :
    public CPlayerState
{
private:
    explicit CPlayer_Interact(shared_ptr<CPlayerBoat> owner,shared_ptr <CPlayerStateMachine> stateMachine);
    virtual ~CPlayer_Interact();

public:
    virtual void Enter()override;
    virtual void Exit()override;


public:
    virtual HRESULT Init_State()override;
    virtual int Update_State(const _float& timeDelta) override;

    virtual void    LateUpdate_State(const _float& timeDelta)override;
    virtual void    Render_State()override;
    

private:

    //weak_ptr<class CSea_Manager> m_pSea_Manager = {};
 
public:
    static shared_ptr<CPlayer_Interact> Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> stateMachine);
   
private:
    EPlayerState m_nextState = EPlayerState::STORAGE;

private:
    virtual void Free();

};

NS_END