//#include "pch.h"
#include "Editor_Win.h"
#include "Win_Manager.h"
#include "GameView.h"
#include "HierarchyWin.h" 
#include "InspectorWin.h" 
#include "ManagerWin.h" 
#include "ConsoleWin.h" 
#include "MapTool.h" 

CWin_Manager::CWin_Manager()
{
}

HRESULT CWin_Manager::Initialize() {
    // 여기서 모든 창을 등록합니다.
    Add_Window(CHierarchyWin::Create());
    Add_Window(CInspectorWin::Create()); 
    Add_Window(CManagerWin::Create());
    Add_Window(CConsoleWin::Create());
    Add_Window(CMapTool::Create());



   // for (auto& pair : m_Windows)
   //     pair.second->Initialize();

    return S_OK;
}

void CWin_Manager::Update(float timeDelta) {
    for (auto& pair : m_windows)
        pair.second->Update(timeDelta);
}

void CWin_Manager::Render() {
    for (auto& pair : m_windows) {
        if (*pair.second->Get_OpenPtr())
            pair.second->Render();
    }
}

void CWin_Manager::Add_Window(shared_ptr<CEditor_Win> window) {
    m_windows[window->Get_Name()] = window;
}

shared_ptr<CEditor_Win> CWin_Manager::Get_Window(string name) {
    auto it = m_windows.find(name);
    return (it != m_windows.end()) ? it->second : nullptr;
}

unique_ptr<CWin_Manager> CWin_Manager::Create()
{
    auto inatance =  make_unique<CWin_Manager>();
    if (FAILED(inatance->Initialize()))
        return nullptr;

        return inatance;
}

void CWin_Manager::Free()
{
}
