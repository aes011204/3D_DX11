#include "UICanvas.h"
#include "GameInstance.h"

CUICanvas::CUICanvas(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    : CUI{ device, context }
{
}

CUICanvas::~CUICanvas()
{
    Free();
}

HRESULT CUICanvas::Initialize(void* arg)
{
    m_canvasSize = m_gameInstance.lock()->Get_WinSize();

    UICanvasDesc* desc = {};
    if (arg != nullptr)
    {
         desc = static_cast<UICanvasDesc*>(arg);

         desc->anchorPoint = { 0.5f, 0.5f }; // 중앙 기준
         desc->pivot = { 0.5f, 0.5f }; // 중앙 기준
         desc->sizeDelta = { m_canvasSize.w, m_canvasSize.h };
         desc->anchoredPos = { 0.f, 0.f };
         desc->scale = { 1.f, 1.f };

         __super::Initialize(arg);
    }
    else
    {
    UICanvasDesc desc;
    // 이거 무조건 고정
    desc.anchorPoint = { 0.5f, 0.5f }; // 중앙 기준
    desc.pivot = { 0.5f, 0.5f }; // 중앙 기준
    desc.sizeDelta = { m_canvasSize.w, m_canvasSize.h };
    desc.anchoredPos = { 0.f, 0.f };
    desc.scale = { 1.f, 1.f };

     __super::Initialize(&desc);


    }

    m_isInteractable = false;

    return S_OK;
}

void CUICanvas::OnUpdate(const _float& timeDelta)
{
   // m_bInteractable = false;
    //m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::UI_WINDOW, static_pointer_cast<CEntity>(shared_from_this()));
}

//void CUICanvas::ResizeCanvasSize(_uint width, _uint height)
//{
//    m_CanvasSize = { 0,0,static_cast<float>(width),static_cast<float>(height) };
//}

shared_ptr<CUICanvas> CUICanvas::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
        
    shared_ptr<CUICanvas> instance ( new CUICanvas(device, context));
    if (FAILED(instance->Initialize_Prototype()))
    {
 
        return nullptr;
    }

    return instance;
}

void CUICanvas::Free()
{
}
