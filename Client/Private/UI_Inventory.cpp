#include "UI_Inventory.h"

CUI_Inventory::CUI_Inventory(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIPanel(pDevice, pContext)
{
}

CUI_Inventory::CUI_Inventory(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

HRESULT CUI_Inventory::OnInit(void* pArg)
{
	INVENTORY_DESC* pDesc = static_cast<INVENTORY_DESC*>(pArg);
	HRESULT hr = {};
	hr = __super::OnInit(pDesc);

	return hr;
}

void CUI_Inventory::OnActive()
{
	__super::OnActive();
		
}

void CUI_Inventory::OnInActive()
{
	__super::OnInActive();

}

void CUI_Inventory::OnDisabled()
{
	__super::OnDisabled();

}

void CUI_Inventory::OnUpdate(const _float& timeDelta)
{
	__super::OnUpdate(timeDelta);

}

void CUI_Inventory::OnLateUpdate()
{
	__super::OnLateUpdate();

}

HRESULT CUI_Inventory::OnRender()
{
	HRESULT hr = {};
	hr = __super::OnRender();

	return hr;
}

void CUI_Inventory::OnClear()
{
	__super::OnClear();

}

shared_ptr<CUI_Inventory> CUI_Inventory::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Inventory> pInstance(new CUI_Inventory(pDevice, pContext), [](CUI_Inventory* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_Inventory::Free()
{
	__super::Free();
}
