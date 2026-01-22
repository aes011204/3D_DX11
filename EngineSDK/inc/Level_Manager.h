#pragma once
#include "Base.h"

NS_BEGIN (Engine)

class CLevel_Manager final : public CBase
{
private:
    CLevel_Manager();
    virtual ~CLevel_Manager() = default;

public:
    HRESULT Change_Level(_uint iNewLevelIndex, class CLevel* pNewLevel);
    void Update(_float fTimeDelta);
    HRESULT Render();

private:
    class CLevel* m_pCurLevel = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };
    _uint m_iCurLevelIdx = {};

public:


    static CLevel_Manager* Create();
    virtual void Free() override;
};

NS_END