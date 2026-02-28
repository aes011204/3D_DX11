#include "UI_MainMenu.h"

CUI_MainMenu::CUI_MainMenu(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUIPanel(pDevice, pContext)
{
}

CUI_MainMenu::CUI_MainMenu(const CUIPanel& prototype)
    : CUIPanel(prototype)
{
}

HRESULT CUI_MainMenu::OnInit(void* pArg)
{
    MAINMENU_DESC
    __super::OnInit(pArg)
    

    return S_OK;
}

void CUI_MainMenu::OnActive()
{
}

void CUI_MainMenu::OnInActive()
{
}

void CUI_MainMenu::OnDisabled()
{
}

void CUI_MainMenu::OnUpdate(const _float& timeDelta)
{
}

void CUI_MainMenu::OnLateUpdate()
{
}

HRESULT CUI_MainMenu::OnRender()
{
    return S_OK;
}

void CUI_MainMenu::OnClear()
{
}

void CUI_MainMenu::Layout()
{
}

void CUI_MainMenu::AddChild()
{
}

shared_ptr<CUI_MainMenu> CUI_MainMenu::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUI_MainMenu> pInstance(new CUI_MainMenu(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIPanel");

    }
    return pInstance;
}

void CUI_MainMenu::Free()
{
    __super::Free();
}
