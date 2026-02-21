#include "UISlot.h"

CUISlot::CUISlot(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIButton(pDevice, pContext)
{
}

CUISlot::CUISlot(const CUISlot& prototype) : CUIButton(prototype)
{
}

HRESULT CUISlot::OnInit(void* pArg)
{
	CUIButton::OnInit( pArg);
	return S_OK;
}

void CUISlot::OnActive()
{
	CUIButton::OnActive();
}

void CUISlot::OnInActive()
{
	CUIButton::OnInActive();
}

void CUISlot::OnDisabled()
{
	CUIButton::OnDisabled();
}

void CUISlot::OnUpdate(const _float& timeDelta)
{
	CUIButton::OnUpdate(timeDelta);
}

void CUISlot::OnLateUpdate()
{
	CUIButton::OnLateUpdate();
}

HRESULT CUISlot::OnRender()
{
	CUIButton::OnRender();
	return S_OK;
}

void CUISlot::OnClear()
{
	CUIButton::OnClear();
}

void CUISlot::Free()
{
	__super::Free();
}
