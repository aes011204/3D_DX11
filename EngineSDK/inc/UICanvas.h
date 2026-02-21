#pragma once
#include "UI.h"


NS_BEGIN(Engine)

class ENGINE_DLL CUICanvas :
    public CUI
{
public:
    struct UICANVAS_DESC : public CUI::UI_DESC
    {
        // canvas는 무조건 투명
        bool EatClick = {true};
    };
private:
    CUICanvas(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
    virtual ~CUICanvas();
    HRESULT Initialize(void* pArg);
    
    virtual void OnUpdate(const _float& timeDelta);

    //void ResizeCanvasSize(_uint width, _uint height);
    //Rect Get_CanvasSize() { return m_CanvasSize; }
private:

    Rect m_CanvasSize = {};
public:
    static shared_ptr<CUICanvas> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END