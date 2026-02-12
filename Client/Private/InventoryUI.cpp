#include "InventoryUI.h"
#include "Inventory_Controller.h"

CInventoryUI::CInventoryUI()
{
}

HRESULT CInventoryUI::Initialize()
{
	return S_OK;
}

void CInventoryUI::Update()
{
}

bool CInventoryUI::MousePosToSlot(_uint& returnSlotX, _uint& returnSlotY)
{
	return true;
}

shared_ptr<CInventoryUI> CInventoryUI::Create()
{
	shared_ptr<CInventoryUI> pInstance ( new CInventoryUI());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CInventoryUI");
	}
	return pInstance;
}

void CInventoryUI::Free()
{
	__super::Free();
}
