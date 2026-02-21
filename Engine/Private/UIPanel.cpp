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

        int Raw = index % m_Col;
        int Col = index / m_Col;

        float posX = m_Padding.x + (m_SlotSize + m_Spacing.x) * Raw;
        float posY = m_Padding.y + (m_SlotSize + m_Spacing.y) * Col;

        child->GetUITransform()->SetAnchoredPos({ posX, posY });

        // child 가 슬랏일 경우 인덱스 를 넘긴다
        dynamic_pointer_cast<CUISlot>(child)->SetGridIndex({ (float)Raw, (float)Col });


        index++;
    }

    // 중점  ///////////////////////////////////////////////////////////////

    float totalWidth = (m_Raw * m_SlotSize) + ((m_Raw - 1) * m_Spacing.x);
    float totalHeight = (m_Raw * m_SlotSize) + ((m_Col - 1) * m_Spacing.y);

    _float2 startPos = { -(totalWidth / 2) - (m_SlotSize / 2), (totalHeight / 2) + (m_SlotSize / 2) };



    for (auto& child : m_Children)
    {
        if (child->IsLayoutTarget() != true)
            return;

        int Raw = index % m_Col;
        int Col = index / m_Col;

        float posX = startPos.x + (m_SlotSize + m_Spacing.x) * Raw;
        float posY = startPos.y + (m_SlotSize + m_Spacing.y) * Col;

        dynamic_pointer_cast<CUISlot>(child)->SetGridIndex({ (float)Raw, (float)Col });

        index++;

    }

}

void CUIPanel::AddChild()
{
}


HRESULT CUIPanel::OnInit(void* pArg)
{
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

shared_ptr<CUIPanel> CUIPanel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUIPanel> pInstance ( new CUIPanel(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIPanel");

    }
    return pInstance;
}

void CUIPanel::Free()
{

    __super::Free();
}
