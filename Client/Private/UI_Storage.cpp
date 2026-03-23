#include "UI_Storage.h"
#include "UISlot.h"

CUI_Storage::CUI_Storage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CUIPanel(pDevice, pContext)
{
}

CUI_Storage::CUI_Storage(const CUIPanel& prototype) :
	CUIPanel(prototype)
{
}



HRESULT CUI_Storage::OnInit(void* pArg)
{

	STORAGE_DESC* pDesc = static_cast<STORAGE_DESC*>(pArg);
	HRESULT hr = {};



	///// ¿ŒπÍ ΩΩ∑‘ ∆–≥Œ /////
	CUIPanel::UIPANEL_DESC Panel_Inven = {};
	Panel_Inven.TextureProtoName = L"Prototype_Component_Texture_StorageInventoryBackground";
	Panel_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
	Panel_Inven.IsFullScreen = false;
	Panel_Inven.IsTransparent = false;
	Panel_Inven.IsUseLayout = true;
	Panel_Inven.bUseNineSlice = false;

	Panel_Inven.LayoutDesc.m_Spacing = { 4.f ,4.f };
	Panel_Inven.LayoutDesc.m_Col = 7;
	Panel_Inven.LayoutDesc.m_Row = 8;
	//Panel_Inven.LayoutDesc.m_Offset = { 0.f, 0.f };


	shared_ptr<CUIPanel> StoragePanel = CUIPanel::Create(m_pDevice, m_pContext);
	StoragePanel->Initialize(&Panel_Inven);
	m_InvenPanel = StoragePanel;

	Add_Child(m_InvenPanel, L"Panel_storage", false);


	/// slot ///
	for (_uint i = 0; i < Panel_Inven.LayoutDesc.m_Col* Panel_Inven.LayoutDesc.m_Row; i++)
	{
		/*for (_uint j = 0; j < w; j++)
		{*/
			CUISlot::SLOT_DESC slot_Inven = {};
			slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
			slot_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
			slot_Inven.vScale = Vector2{ 1.6f,1.6f };

			slot_Inven.slotType = ETOI(SLOT_TYPE::ANY);

			shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_pDevice, m_pContext);
			m_pInstanceINVEN->Initialize(&slot_Inven);

			wstring NameTag = L"SLOT_STORAGE_" + S2W(to_string(i));


			m_InvenPanel->Add_Layout_Child(m_pInstanceINVEN, NameTag, false);
		//}
	}





	hr = __super::OnInit(pDesc);


    return hr;
}

void CUI_Storage::OnActive()
{
	__super::OnActive();

}

void CUI_Storage::OnInActive()
{
	__super::OnActive();

}

void CUI_Storage::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_Storage::OnUpdate(const _float& timeDelta)
{
	__super::OnUpdate(timeDelta);
}

void CUI_Storage::OnLateUpdate()
{
	__super::OnLateUpdate();
}


HRESULT CUI_Storage::OnRender()
{

    return __super::OnRender();
}

void CUI_Storage::OnClear()
{
	__super::OnClear();
}

shared_ptr<CUI_Storage> CUI_Storage::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Storage> pInstance(new CUI_Storage(pDevice, pContext), [](CUI_Storage* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIStorage");

	}
	return pInstance;
}

void CUI_Storage::Free()
{
	__super::Free();
}
