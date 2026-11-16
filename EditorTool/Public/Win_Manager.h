#pragma once
#include "Editor_Define.h"

NS_BEGIN(Editor)

class CEditor_Win;

class CWin_Manager
{
public:
    CWin_Manager();
    virtual ~CWin_Manager() = default;
public:
    HRESULT Initialize();
    void Update(float timeDelta);
    void Render();

    void Add_Window(shared_ptr<CEditor_Win> window);
    shared_ptr<CEditor_Win> Get_Window(string name);

private:
    // 창 이름(_string)을 키값으로 관리하는 컨테이너
    map<_string, shared_ptr<CEditor_Win>> m_windows;

public:
    static unique_ptr<CWin_Manager> Create();
    void Free();
};

NS_END