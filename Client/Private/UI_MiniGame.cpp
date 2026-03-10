#include "UI_MiniGame.h"

CUI_MiniGame::CUI_MiniGame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIPanel(pDevice, pContext)
{
}

CUI_MiniGame::CUI_MiniGame(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

HRESULT CUI_MiniGame::OnInit(void* pArg)
{
	return CUIPanel::OnInit(pArg);
}

void CUI_MiniGame::OnActive()
{
	CUIPanel::OnActive();
}

void CUI_MiniGame::OnInActive()
{
	CUIPanel::OnInActive();
}

void CUI_MiniGame::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_MiniGame::OnUpdate(const _float& timeDelta)
{
	CUIPanel::OnUpdate(timeDelta);
}

void CUI_MiniGame::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_MiniGame::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_MiniGame::OnClear()
{
	CUIPanel::OnClear();
}

shared_ptr<CUI_MiniGame> CUI_MiniGame::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_MiniGame> pInstance(new CUI_MiniGame(pDevice, pContext), [](CUI_MiniGame* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_MiniGame::Free()
{
	CUIPanel::Free();
}
