#include "UISlot.h"

CUISlot::CUISlot(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CUIButton(device, context)
{
}

CUISlot::CUISlot(const CUISlot& prototype) : CUIButton(prototype)
{
}

void CUISlot::Change_Color(_fvector color, bool useDark, bool useColorMix, bool useColorFlat)
{

}

HRESULT CUISlot::OnInit(void* arg)
{
	SlotDesc* desc = static_cast<SlotDesc*>(arg);
	m_slotType = desc->slotType;
	m_gridIndex = desc->index;


	CUIButton::OnInit(desc);

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

shared_ptr<CUISlot> CUISlot::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUISlot> instance(new CUISlot(device, context), [](CUISlot* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUISlot");
		return nullptr;
	}
	return instance;

}


void CUISlot::Free()
{
	__super::Free();
}
