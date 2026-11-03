#pragma once
#include "PlayerState.h"


namespace Engine
{
	class CLight;
}

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

    weak_ptr<class CSea_Manager> m_seaManager = {};
   
private:
    EPlayerState m_nextState = EPlayerState::SEA;

    //float m_TargetSpeed = {};
    float m_curSpeed = {};
    float m_maxSpeed = {};
    float m_acceleration = {};
    float m_deceleration = {}; // 아무것도 안할떄 줄어드는 양

    float m_curTurnSpeed = {};
    float m_maxTurnSpeed = {};
    float m_turnAcceleration = {};
    float m_turnDeceleration = {};
   ///
    bool m_isDocking = false;
    bool m_isFinDock = false;
    _float3 m_targetPos = {};
    weak_ptr<CLight> m_lightObj = {  };
    bool m_isOnTab = false;
    bool On_Light = false;
    bool bPlaying = false;

    weak_ptr<class CUI_TabContainer> m_tabUi={};
public:
    static shared_ptr<CPlayer_Sea> Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine);

private:
    virtual void Free();

};

NS_END