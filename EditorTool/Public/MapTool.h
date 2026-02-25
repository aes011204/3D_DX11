#pragma once
#include "Editor_Win.h"


NS_BEGIN(Engine)
class CGameObject;
class CData_Manager;
NS_END

NS_BEGIN(Editor)

class CMapTool final:
    public CEditor_Win
{
public:
    CMapTool();
    virtual ~CMapTool();

public:
    virtual HRESULT Initialize() override;
    virtual void Update(float fTimeDelta) override;
    virtual void Render() override;

    void Render_SaveLoadPanel();

private:
public:
    static shared_ptr<CMapTool> Create();
};

NS_END