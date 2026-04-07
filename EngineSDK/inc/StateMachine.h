#pragma once
#include "Base.h"


NS_BEGIN(Engine)

class CState;
class ENGINE_DLL CStateMachine :
    public CBase
{
protected:
    CStateMachine();
public:
    ~CStateMachine() override;

public:
    virtual HRESULT Init_StateMachine() = 0;
    virtual _int    Update_StateMachine(const _float& timeDelta) = 0;
    virtual void    Change_State(_uint dwStateKey) = 0;

    _uint       Get_CurStateNum() { return m_CurStateKey; }
    _uint       Get_PrevStateKey() { return m_PrevStateKey; }
    shared_ptr<CState> Get_CurState() { return m_CurState; } // 소유권 없음 row

protected:
    vector<shared_ptr<CState>> m_vecState;
    shared_ptr< CState> m_CurState;
    // shared_ptr<CState> m_pPrevState;

    _uint m_PrevStateKey;
    _uint m_CurStateKey;

protected:
    virtual void Free() override;
};

NS_END