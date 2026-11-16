#pragma once
#include "State.h"
#include "Client_Define.h"


namespace Engine
{

	class CCollider;
	class CDInput_Manager;
	class CGameInstance;
}

NS_BEGIN(Client)
class CCamera_Play;
	class CPlayerStateMachine;

class CPlayerBoat;

class CPlayerState :
	public CState
{
protected:
	explicit CPlayerState(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> stateMachine);
public:
	virtual ~CPlayerState();

public:
	virtual void Enter();
	virtual void Exit();

public:
	virtual HRESULT Init_State() override;
	//virtual _int    Update_State(const _float& timeDelta) { return 0; }
	//virtual void    LateUpdate_State(const _float& timeDelta) {}
	//virtual void    Render_State() {}


protected:
	virtual void    Find_Player();
protected:

	_bool       m_isAnimDone = false;

	weak_ptr<CPlayerBoat> m_owner = {};
	weak_ptr<CGameInstance> m_gameInstance;
	weak_ptr < CPlayerStateMachine> m_stateMachine = { };

	weak_ptr<CTransform> m_ownerTransformCom = { };
	
	CDInput_Manager* m_inputManager = { nullptr };




	list<string> m_dialogueList;
private:
	// CBase을(를) 통해 상속됨
	//void Free() override;

};

NS_END