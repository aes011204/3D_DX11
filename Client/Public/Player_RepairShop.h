#pragma once
#include "PlayerState.h"


NS_BEGIN(Client)

	class CPlayerBoat;
	class CPlayerStateMachine;
class CPlayer_RepairShop :
    public CPlayerState
{
private:
    explicit CPlayer_RepairShop(shared_ptr<CPlayerBoat> owner,shared_ptr <CPlayerStateMachine> pStateMachine);
    virtual ~CPlayer_RepairShop();

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
    static shared_ptr<CPlayer_RepairShop> Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine);
   
private:
    E_PLAYERSTATE eNextState = E_PLAYERSTATE::REPAIR_SHOP;


private:
    virtual void Free();

};

NS_END