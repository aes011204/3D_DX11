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
    virtual void Update(float timeDelta) override;
    virtual void Render() override;

    void Render_SaveLoadPanel(ESaveType type, const string& folderPath);

private:
    vector<string> s_files[(int)ESaveType::END];
    int            s_selected[(int)ESaveType::END] = { -1, -1 };
    char           s_newFileName[128] = "NewData.json";
public:
    static shared_ptr<CMapTool> Create();
};

NS_END