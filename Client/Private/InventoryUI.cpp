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

CInventoryUI* CInventoryUI::Create()
{
	CInventoryUI* pInstance = new CInventoryUI();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CInventoryUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventoryUI::Free()
{
	__super::Free();
}
