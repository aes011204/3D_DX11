#include "UIButton.h"
#include "DInput_Manager.h"

CUIButton::CUIButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUI(pDevice, pContext)
{
}

CUIButton::CUIButton(const CUIButton& prototype)
	: CUI(prototype)
{
}

void CUIButton::OnInit()
{
}

void CUIButton::OnActive()
{
}

void CUIButton::OnInActive()
{
}

void CUIButton::OnDisabled()
{
    m_UIState = BUTTON_STATE::DISABLE;


}

void CUIButton::OnUpdate(const _float& timeDelta)
{
    ProcessInput();

    switch (m_UIState)
    {
    case BUTTON_STATE::CLICK:
        break;
    case BUTTON_STATE::HOVER:
        break;
    case BUTTON_STATE::NONE:
        break;
    case BUTTON_STATE::DISABLE: // 상점등에서 보이는데 돈이 없어서 클릭 할수 없는 상태
        break;

    }
}

void CUIButton::OnLateUpdate()
{
}

void CUIButton::OnRender()
{
}

void CUIButton::OnClear()
{
}

void CUIButton::ProcessInput()
{
    // 부모가 마우스 충돌+가로채기 검사를 끝

    bool mouseDown = CDInput_Manager::GetInstance()->MouseDown(MOUSEKEYSTATE::DIM_LB);
    bool mouseUp = CDInput_Manager::GetInstance()->MouseUp(MOUSEKEYSTATE::DIM_LB);

    
        if (m_UIState != BUTTON_STATE::DISABLE)
        {
            return;
        }



    if (m_bHovered)
    {
        if (mouseDown) // 방금 클릭
        {
            m_ClickInside = true;
            m_UIState = BUTTON_STATE::CLICK;
        }
        else if (mouseUp && m_ClickInside) // 안에서 클릭한 상태에서 안에서 뗏는지
        {
            m_ClickInside = false;
            m_UIState = BUTTON_STATE::HOVER;

            if (m_ClickEvent) // 콜백 실행
            {
                m_ClickEvent();
            }
        }
        else if (!mouseDown && m_UIState != BUTTON_STATE::CLICK) // 호버링 중인가
        {
            m_UIState = BUTTON_STATE::HOVER;
        }
    }
    else
    {
        if (mouseUp)
            m_ClickInside = false;

        m_UIState = BUTTON_STATE::NONE;
    }


}

void CUIButton::ChangeState(BUTTON_STATE next)
{
}

shared_ptr<CUIButton> CUIButton::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUIButton> pInstance (new CUIButton(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIButton");
    }
    return pInstance;
}

void CUIButton::Free()
{
    __super::Free();
}
