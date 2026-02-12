#include "UICanvas.h"

CUICanvas::CUICanvas()
{
}

CUICanvas::~CUICanvas()
{
    Free();
}

HRESULT CUICanvas::Initialize(_uint width, _uint height)
{
    m_CanvasSize = { 0,0,static_cast<float>(width),static_cast<float>(height) };

    return S_OK;
}

void CUICanvas::ResizeCanvasSize(_uint width, _uint height)
{
    m_CanvasSize = { 0,0,static_cast<float>(width),static_cast<float>(height) };
}

unique_ptr<CUICanvas> CUICanvas::Create(_uint width, _uint height)
{
        
    unique_ptr<CUICanvas> pInstance ( new CUICanvas());
    if (FAILED(pInstance->Initialize(width, height)))
    {
        
        return nullptr;
    }

    return pInstance;
}

void CUICanvas::Free()
{
}
