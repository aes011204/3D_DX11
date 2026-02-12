#pragma once
#include "UI.h"


NS_BEGIN(Engine)

class CUICanvas :
    public CBase
{
private:
    CUICanvas();
public:
    virtual ~CUICanvas();

    HRESULT Initialize(_uint width, _uint height);
    
    void ResizeCanvasSize(_uint width, _uint height);
    Rect Get_CanvasSize() { return m_CanvasSize; }
private:

    Rect m_CanvasSize = {};
public:
    static unique_ptr<CUICanvas> Create(_uint width, _uint height);
    void Free() override;
};

NS_END