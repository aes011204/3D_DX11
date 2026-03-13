#pragma once
#include "Editor_Win.h"

NS_BEGIN(Engine)
class CGameObject;
class CUI;
class CEntity;
NS_END

NS_BEGIN(Editor)

class CHierarchyWin final :
    public CEditor_Win
{
public:
    CHierarchyWin();
    virtual ~CHierarchyWin();



public:
    virtual HRESULT Initialize() override;
    virtual void Update(float fTimeDelta) override;
    virtual void Render() override;

    void DrawUITree(const shared_ptr<Engine::CUI>& ui, _string imguiLabel);

private:
    // ===== UI state =====
    shared_ptr<Engine::CEntity> m_pSelectedObject = nullptr;

    // Rename state
    uint64_t m_RenameTargetPtr = 0;     // 임시: 포인터 기반 (나중에 UUID로 교체 추천)
    char     m_RenameBuf[256] = {};

    // Filter
    char     m_FilterBuf[128] = {};

private:
    // ===== draw helpers =====
    void DrawObjectRow(const shared_ptr<Engine::CGameObject>& pObj);
    bool PassFilter(const shared_ptr<Engine::CGameObject>& pObj) const;

    void Engine_DeleteObject(const shared_ptr<Engine::CGameObject>& pObj) {};

    // Rename commit helper (중복 처리하고 싶으면 여기서 처리)
    void CommitRename(const shared_ptr<Engine::CGameObject>& pObj, const char* newNameUtf8);

    // util: ImGui 내부 ID(충돌 방지)
    uint64_t PtrID(const shared_ptr<Engine::CGameObject>& pObj) const;

public:
    static shared_ptr<CHierarchyWin> Create();
};

NS_END