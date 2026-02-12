#pragma once
#include "Editor_Define.h"

NS_BEGIN(Editor)

class CEditor_Win;

class CWin_Mananger
{
public:
    CWin_Mananger();
    virtual ~CWin_Mananger() = default;
public:
    HRESULT Initialize();
    void Update(float fTimeDelta);
    void Render();

    void Add_Window(shared_ptr<CEditor_Win> pWindow);
    shared_ptr<CEditor_Win> Get_Window(string name);

private:
    // 창 이름(_string)을 키값으로 관리하는 컨테이너
    map<_string, shared_ptr<CEditor_Win>> m_Windows;

public:
    static unique_ptr<CWin_Mananger> Create();
    void Free();
};

NS_END