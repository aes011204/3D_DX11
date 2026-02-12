#pragma once
#include "Editor_Define.h"

NS_BEGIN(Editor)

class CEditor_Win abstract 
{
public:
    CEditor_Win(_string name);
    virtual ~CEditor_Win() = default;

    virtual HRESULT Initialize() ;
    virtual void Update(float fTimeDelta) ;
    virtual void Render() ; // 여기서 ImGui 코드를 작성합니다.


public:
    bool* Get_OpenPtr() { return &m_bOpen; }
    _string Get_Name() { return m_strName; }

protected:
    _string m_strName; // 창의 고유 이름 (ID 겸용)
    bool m_bOpen = true;

public:
    virtual void Free();
};

NS_END