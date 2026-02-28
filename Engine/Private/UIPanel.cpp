#include "UIPanel.h"
#include "UISlot.h"
#include "UITransform.h"

CUIPanel::CUIPanel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI(pDevice, pContext)
{

}

CUIPanel::CUIPanel(const CUIPanel& prototype)
    : CUI(prototype)
{
}

void CUIPanel::Layout()
{

    // 일단 left top
    for (auto& child : m_Children)
    {
        if (child->IsLayoutTarget() != true)
            return;

        int Raw = index % m_LayoutDesc.m_Col;
        int Col = index / m_LayoutDesc.m_Col;

        float posX = m_LayoutDesc.m_Padding.x + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.x) * Raw;
        float posY = m_LayoutDesc.m_Padding.y + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.y) * Col;

        child->GetUITransform()->SetAnchoredPos({ posX, posY });

        // child 가 슬랏일 경우 인덱스 를 넘긴다
        dynamic_pointer_cast<CUISlot>(child)->SetGridIndex({ (float)Raw, (float)Col });


        index++;
    }

    // 중점  ///////////////////////////////////////////////////////////////

    float totalWidth = (m_LayoutDesc.m_Raw * m_LayoutDesc.m_SlotSize) + ((m_LayoutDesc.m_Raw - 1) * m_LayoutDesc.m_Spacing.x);
    float totalHeight = (m_LayoutDesc.m_Raw * m_LayoutDesc.m_SlotSize) + ((m_LayoutDesc.m_Col - 1) * m_LayoutDesc.m_Spacing.y);

    _float2 startPos = { -(totalWidth / 2) - (m_LayoutDesc.m_SlotSize / 2), (totalHeight / 2) + (m_LayoutDesc.m_SlotSize / 2) };



    for (auto& child : m_Children)
    {
        if (child->IsLayoutTarget() != true)
            return;

        int Raw = index % m_LayoutDesc.m_Col;
        int Col = index / m_LayoutDesc.m_Col;

        float posX = startPos.x + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.x) * Raw;
        float posY = startPos.y + (m_LayoutDesc.m_SlotSize + m_LayoutDesc.m_Spacing.y) * Col;

        dynamic_pointer_cast<CUISlot>(child)->SetGridIndex({ (float)Raw, (float)Col });

        index++;

    }

}

void CUIPanel::AddChild()
{
}


HRESULT CUIPanel::OnInit(void* pArg)
{
    UIPANEL_DESC* pDesc = static_cast<UIPANEL_DESC*>(pArg);

    m_LayoutDesc = pDesc->LayoutDesc;

    // 이건 부모가 CUI라서 할필요 없는데 그냥 ㄱㄱ
    __super::OnInit(pDesc);

    return S_OK;
}

void CUIPanel::OnActive()
{
}

void CUIPanel::OnInActive()
{
}

void CUIPanel::OnDisabled()
{
}

void CUIPanel::OnUpdate(const _float& timeDelta)
{
}

void CUIPanel::OnLateUpdate()
{
}

HRESULT CUIPanel::OnRender()
{
    return S_OK;

}

void CUIPanel::OnClear()
{
}

void CUIPanel::Free()
{

    __super::Free();
}
