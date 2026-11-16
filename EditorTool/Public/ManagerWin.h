#pragma once
#include "Editor_Win.h"

NS_BEGIN(Engine)
class CBase;
class CGameObject;

NS_END


NS_BEGIN(Editor)

class CManagerWin final :
    public CEditor_Win
{
public:
    CManagerWin();
    virtual ~CManagerWin();

public:
    virtual HRESULT Initialize() override;
    virtual void Update(float timeDelta) override;
    virtual void Render() override;

    void DrawUITree(const Engine::CBase& UI);

    bool Picking();

private:
    _uint m_selectedLevel = {};
    wstring m_selectedTag = {};

    _bool m_isPickMode = false;
    shared_ptr<CGameObject> m_preview = {};
public:
    static shared_ptr<CManagerWin> Create();

};

NS_END