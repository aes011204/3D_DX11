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
    virtual _uint Get_StateKey() const { return m_dwCurStateKey; }

protected:
    _uint m_dwCurStateKey = { 0 };

    _bool m_bStarted = {false};
    _bool m_bExited = { false };
    _bool m_bCanTransit = { false };


    weak_ptr<CTransform> m_pOwnerTransformCom = { };
   


    _float      m_fCurStateElapsedTime = { 0.f }; // 누적시간
    _float      m_fMinStateTime = { 0.f };
    _bool       m_bMinStateTimePassed = { false };


};

NS_END