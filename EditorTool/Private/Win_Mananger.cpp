//#include "pch.h"
#include "Editor_Win.h"
#include "Win_Mananger.h"
#include "GameView.h"
#include "HierarchyWin.h" 
#include "InspectorWin.h" 
#include "ManagerWin.h" 
#include "ConsoleWin.h" 
#include "MapTool.h" 

CWin_Mananger::CWin_Mananger()
{
}

HRESULT CWin_Mananger::Initialize() {
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

void CWin_Mananger::Update(float fTimeDelta) {
    for (auto& pair : m_Windows)
        pair.second->Update(fTimeDelta);
}

void CWin_Mananger::Render() {
    for (auto& pair : m_Windows) {
        if (*pair.second->Get_OpenPtr())
            pair.second->Render();
    }
}

void CWin_Mananger::Add_Window(shared_ptr<CEditor_Win> pWindow) {
    m_Windows[pWindow->Get_Name()] = pWindow;
}

shared_ptr<CEditor_Win> CWin_Mananger::Get_Window(string name) {
    auto it = m_Windows.find(name);
    return (it != m_Windows.end()) ? it->second : nullptr;
}

unique_ptr<CWin_Mananger> CWin_Mananger::Create()
{
    auto pInatance =  make_unique<CWin_Mananger>();
    if (FAILED(pInatance->Initialize()))
        return nullptr;

        return pInatance;
}

void CWin_Mananger::Free()
{
}
