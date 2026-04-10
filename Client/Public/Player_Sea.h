#pragma once
#include "PlayerState.h"


NS_BEGIN(Client)

	class CPlayerBoat;
	class CPlayerStateMachine;
class CPlayer_Sea :
    public CPlayerState
{
private:
    explicit CPlayer_Sea(shared_ptr<CPlayerBoat> owner,shared_ptr <CPlayerStateMachine> pStateMachine);
    virtual ~CPlayer_Sea();

public:
    virtual void Enter()override;
    virtual void Exit()override;


public:
    virtual HRESULT Init_State()override;
    virtual int Update_State(const _float& timeDelta) override;

    virtual void    LateUpdate_State(const _float& timeDelta)override;
    virtual void    Render_State()override;




    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;



    void Location_Sea(_float fTimeDelta);
   // bool Docking(_float fTimeDelta,_float3 targetPos);
    _uint Move(_float fTimeDelta);

private:

    weak_ptr<class CSea_Manager> m_pSea_Manager = {};
   
private:
    PLAYERSTATE m_NextState = PLAYERSTATE::SEA;

    //float m_TargetSpeed = {};
    float m_CurSpeed = {};
    float m_MaxSpeed = {};
    float m_Acceleration = {};
    float m_Deceleration = {}; // 아무것도 안할떄 줄어드는 양
   ///
    bool m_bIsDocking = false;
    bool m_bFinDock = false;
    _float3 m_targetPos = {};
public:
    static shared_ptr<CPlayer_Sea> Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine);

private:
    virtual void Free();

};

NS_END