#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLevel;

class CLevel_Manager final : public CBase
{
private:
    CLevel_Manager();
public:
    virtual ~CLevel_Manager() = default;

public:
    HRESULT Initialize(_uint EditorIndex, _uint iNumLevel);
    HRESULT Change_Level(_uint iNewLevelIndex, shared_ptr<CLevel> pNewLevel);
    void Update(_float fTimeDelta);
    HRESULT Render();



/// <imgui>
    _uint Get_CurLevelIdx() { return m_iCurLevelIdx; }
/// </summary>
private:
    shared_ptr<CLevel> m_pCurLevel = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = { };
    _uint m_iCurLevelIdx = {};
    _uint m_iEditorIndex = {};

    _uint m_iNumLevel = {};

public:


    static unique_ptr<CLevel_Manager> Create(_uint EditorIndex, _uint iNumLevel);
    virtual void Free() override;
};

NS_END