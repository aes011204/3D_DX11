#pragma once
#include "Editor_Win.h"

NS_BEGIN(Engine)
class CBase;

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
    virtual void Update(float fTimeDelta) override;
    virtual void Render() override;

    void DrawUITree(const Engine::CBase& ui);



public:
    static shared_ptr<CManagerWin> Create();

};

NS_END