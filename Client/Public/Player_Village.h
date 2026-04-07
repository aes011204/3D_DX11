#pragma once
#include "PlayerState.h"


NS_BEGIN(Client)

	class CPlayerBoat;
	class CPlayerStateMachine;
class CPlayer_Village :
    public CPlayerState
{
private:
    explicit CPlayer_Village(shared_ptr<CPlayerBoat> owner,shared_ptr <CPlayerStateMachine> pStateMachine);
    virtual ~CPlayer_Village();

public:
    virtual void Enter()override;
    virtual void Exit()override;


public:
    virtual HRESULT Init_State()override;
    virtual int Update_State(const _float& timeDelta) override;

    virtual void    LateUpdate_State(const _float& timeDelta)override;
    virtual void    Render_State()override;
    void Location_Sea(_float fTimeDelta);

private:

    //weak_ptr<class CSea_Manager> m_pSea_Manager = {};
 
public:
    static shared_ptr<CPlayer_Village> Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine);
   
private:
   

private:
    virtual void Free();

};

NS_END