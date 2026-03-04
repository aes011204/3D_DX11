#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CUI;
class CUICanvas;

class CUI_Manager final :
    public CBase
{
public:
    struct RequestUI
    {
        wstring UIKey;      
        UI_LAYER Layer;
        void* pArg;
    };
private:
    CUI_Manager();
public:
    virtual ~CUI_Manager();

public:
    HRESULT Initialize(_uint width, _uint height);
    void Update(float m_fDeltaTime); //  request 요청 처리기 (ui 업뎃 돌리지 않음)
    void Late_Update(float m_fDeltaTime);
    void Render(); // 안쓸듯 이거는 렌더러에서 하니까

    void Push(UI_LAYER layer, wstring name, _bool isOnActive = true, void* pArg = nullptr);
    void Pop(UI_LAYER layer,wstring type); // 레이어에서 넣얶다 뻇다하는건 안씀 

    shared_ptr<CUI> Find_UI_InCurLevel(UI_LAYER layer, wstring type);

    void Detach_All(); // 씬 전환 할떄 레이어에 있는거 객체를 지우니까 그전에 

    void InsertToPool(wstring UIType, shared_ptr<CUI> UI);

    Rect Get_WinSize();

    ///IMGUI///
    const vector<shared_ptr<CUI>>& GetUIList(UI_LAYER layer)
    {
        return m_UI[ETOI(layer)];
    }
    const unordered_map<wstring, shared_ptr<CUI>>& GetUIPool()
    {
        return m_UIPool;
    }
    shared_ptr<CUI> GetActiveUI() const { return m_ActiveUI; }



    const wstring& GetUIKey() const { return m_UIKey; }
    void SetUIKey(const wstring& key) { m_UIKey = key; }

    virtual void OnGui() override;
private:

    void DrawUITree(const shared_ptr<Engine::CUI>& ui);
    wstring m_UIKey;
    shared_ptr<CUI> m_pSelectedObject;

    ///IMGUI///


private:
    shared_ptr<CUI> FindUIOnPool(wstring type); // 해당 UI가 있는지 확인 있으면 해단 ui 반환
    //void OnComplete(UI_LAYER layer, shared_ptr<CUI> addUI, void* pArg); // 할거 하고   push 
  //  void ProcessUIQ();
    void OnFail();

private:

    unordered_map<wstring, shared_ptr<CUI>> m_UIPool = {}; // 만들어진 UI를 소유하고 있는 창고(풀)

    vector<shared_ptr<CUI>> m_UI[ETOI(UI_LAYER::END)] = {};

	//queue<RequestUI> m_RequestUIQueue = {}; // UI 전환 요청 큐 (씬 매니져의 요청 하는거랑 같음)
    shared_ptr<CUI> m_ActiveUI = nullptr; // 보통 m_UIBases.top()과 동일

    Rect m_Winsize = {};

   
public:
    static unique_ptr<CUI_Manager> Create(_uint width, _uint height);
    void Free() override;

};


NS_END
