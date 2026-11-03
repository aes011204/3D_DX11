#pragma once
#include "Base.h"
#include "Transform.h"
NS_BEGIN(Engine)

class CCollider;

	class ENGINE_DLL CState abstract :
    public CBase
{
protected:
    explicit CState();
public:
    virtual ~CState();

public:
    virtual void Enter();
    virtual void Exit();


public:
    virtual HRESULT Init_State() { return S_OK; }
    virtual int Update_State(const _float& timeDelta) { return 0; }
    virtual void    LateUpdate_State(const _float& timeDelta) {}
    virtual void    Render_State() {}


    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) {};
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) {};
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) {};


public:
    virtual _uint Get_StateKey() const { return m_curStateKey; }

protected:
    _uint m_curStateKey = { 0 };

    _bool m_isStarted = {false};
    _bool m_isExited = { false };
    _bool m_canTransit = { false };


    weak_ptr<CTransform> m_ownerTransformCom = { };
   


    _float      m_curStateElapsedTime = { 0.f }; // 누적시간
    _float      m_minStateTime = { 0.f };
    _bool       m_isMinStateTimePassed = { false };


};

NS_END