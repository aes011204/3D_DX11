#include "UISlot.h"

CUISlot::CUISlot(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIButton(pDevice, pContext)
{
}

CUISlot::CUISlot(const CUISlot& prototype) : CUIButton(prototype)
{
}

void CUISlot::OnInit()
{
	CUIButton::OnInit();
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

void CUISlot::OnRender()
{
	CUIButton::OnRender();
}

void CUISlot::OnClear()
{
	CUIButton::OnClear();
}

void CUISlot::Free()
{
	__super::Free();
}
