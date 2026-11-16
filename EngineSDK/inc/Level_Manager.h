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
    HRESULT Initialize(_uint editorIndex, _uint numLevel);
	
    HRESULT Change_Level(_uint newLevelIndex, shared_ptr<CLevel> newLevel);

	
    void Update(_float timeDelta);
    HRESULT Render();



/// <imgui>
    _uint Get_CurLevelIdx() { return m_curLevelIdx; }
/// </summary>
private:
    shared_ptr<CLevel> m_curLevel = { nullptr };
    weak_ptr<class CGameInstance> m_gameInstance = { };
    _uint m_curLevelIdx = {};
    _uint m_editorIndex = {};

    _uint m_numLevel = {};

public:


    static unique_ptr<CLevel_Manager> Create(_uint editorIndex, _uint numLevel);
    virtual void Free() override;
};

NS_END