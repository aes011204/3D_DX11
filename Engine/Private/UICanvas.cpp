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
    m_CanvasSize = m_pGameInstance.lock()->Get_WinSize();

    UICANVAS_DESC* pDesc = {};
    if (pArg != nullptr)
    {
         pDesc = static_cast<UICANVAS_DESC*>(pArg);

         pDesc->vAnchorPoint = { 0.5f, 0.5f }; // 중앙 기준
         pDesc->vPivot = { 0.5f, 0.5f }; // 중앙 기준
         pDesc->vSizeDelta = { m_CanvasSize.w, m_CanvasSize.h };
         pDesc->vAnchoredPos = { 0.f, 0.f };
         pDesc->vScale = { 1.f, 1.f };

         __super::Initialize(pArg);
    }
    else
    {
    UICANVAS_DESC pDesc;
    // 이거 무조건 고정
    pDesc.vAnchorPoint = { 0.5f, 0.5f }; // 중앙 기준
    pDesc.vPivot = { 0.5f, 0.5f }; // 중앙 기준
    pDesc.vSizeDelta = { m_CanvasSize.w, m_CanvasSize.h };
    pDesc.vAnchoredPos = { 0.f, 0.f };
    pDesc.vScale = { 1.f, 1.f };

     __super::Initialize(&pDesc);


    }

    m_bInteractable = false;

    return S_OK;
}

void CUICanvas::OnUpdate(const _float& timeDelta)
{
   // m_bInteractable = false;
    m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::UI, static_pointer_cast<CEntity>(shared_from_this()));
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
