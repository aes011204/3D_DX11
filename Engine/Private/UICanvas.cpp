#include "UICanvas.h"
#include "GameInstance.h"

CUICanvas::CUICanvas(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI{ pDevice, pContext }
{
}

CUICanvas::~CUICanvas()
{
    Free();
}

HRESULT CUICanvas::Initialize(void* pArg)
{
    m_canvasSize = m_gameInstance.lock()->Get_WinSize();

    UICanvasDesc* pDesc = {};
    if (pArg != nullptr)
    {
         pDesc = static_cast<UICanvasDesc*>(pArg);

         pDesc->anchorPoint = { 0.5f, 0.5f }; // 중앙 기준
         pDesc->pivot = { 0.5f, 0.5f }; // 중앙 기준
         pDesc->sizeDelta = { m_canvasSize.w, m_canvasSize.h };
         pDesc->anchoredPos = { 0.f, 0.f };
         pDesc->scale = { 1.f, 1.f };

         __super::Initialize(pArg);
    }
    else
    {
    UICanvasDesc pDesc;
    // 이거 무조건 고정
    pDesc.anchorPoint = { 0.5f, 0.5f }; // 중앙 기준
    pDesc.pivot = { 0.5f, 0.5f }; // 중앙 기준
    pDesc.sizeDelta = { m_canvasSize.w, m_canvasSize.h };
    pDesc.anchoredPos = { 0.f, 0.f };
    pDesc.scale = { 1.f, 1.f };

     __super::Initialize(&pDesc);


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

shared_ptr<CUICanvas> CUICanvas::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
        
    shared_ptr<CUICanvas> pInstance ( new CUICanvas(pDevice, pContext));
    if (FAILED(pInstance->Initialize_Prototype()))
    {
 
        return nullptr;
    }

    return pInstance;
}

void CUICanvas::Free()
{
}
