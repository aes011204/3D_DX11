#include "UI_Inventory.h"
#include "UIImage.h"
#include "UISlot.h"
#include "Inventory_Controller.h"
#include "EventBus.h"
#include "Client_Enum.h"

CUI_Inventory::CUI_Inventory(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIPanel(pDevice, pContext)
{
}

CUI_Inventory::CUI_Inventory(const CUIPanel& prototype)
	: CUIPanel(prototype)
{

}

HRESULT CUI_Inventory::Initialize_Prototype()
{
	// 이건 그냥  PreInitialize 대용으로 사용
	//
	///발행이 나중에 되야함 
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_UIslot_Data>([this](const Evt_UIslot_Data& e) {Rebuild_InventorySlot(e.w, e.h, e.InvenSlot);});

	///


	return CUIPanel::Initialize_Prototype();
}

_bool CUI_Inventory::Rebuild_InventorySlot(_uint w,_uint h, const vector<Slot>& inven_slot)
{
	//보트 슬롯모양이 바뀌면 람다등으로 Init_InventorySlot 호출 
	//find boat back ground ->  clear boat back ground's chilren -> for{for{}} add new slot children

	if (/*Inven_Contrl == nullptr||*/ m_InvenPanel==nullptr)
	{
		MSG_BOX("failed  : Rebuild_InventorySlot");
		return false;
	}



	m_InvenPanel->UI_Clear();


	m_InvenPanel->Change_LayoutRawCol(w, h);


	/// slot ///


	for(_uint i =0; i < h; i++)
	{
		for (_uint j = 0; j < w; j++)
		{
			CUISlot::SLOT_DESC slot_Inven = {};
			slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
			slot_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
			slot_Inven.vScale = Vector2{ 1.6f,1.6f };

			if(inven_slot[i*w+j].IsLock == true)
			{
				slot_Inven.IsTransparent = true;
			}
			slot_Inven.slotType = ETOI(inven_slot[i * w + j].slotType);
			
			shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_pDevice, m_pContext);
			m_pInstanceINVEN->Initialize(&slot_Inven);

			wstring NameTag = L"SLOT_INVEN_" + S2W(to_string(i * w + j));


			m_InvenPanel->Add_Layout_Child(m_pInstanceINVEN, NameTag, false);
		}
	}


	return true;
}

HRESULT CUI_Inventory::OnInit(void* pArg)
{
	
	INVENTORY_DESC* pDesc = static_cast<INVENTORY_DESC*>(pArg);
	HRESULT hr = {};



	///// 정보 이미지 /////

	CUIImage::UIIMAGE_DESC ImgDesc = {};
	ImgDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	ImgDesc.TextureProtoName = L"Prototype_Component_Texture_PanelBackground";
	ImgDesc.ZOrder = 3;

	shared_ptr<CUIImage> pChild = CUIImage::Create(m_pDevice, m_pContext);
	pChild->Initialize(&ImgDesc);

	wstring NameTag = L"INVEN_BACK";



	Add_Child(pChild, NameTag, false);


	///// 인밴 슬롯 패널 /////
	CUIPanel::UIPANEL_DESC Panel_Inven = {};
	Panel_Inven.TextureProtoName = L"Prototype_Component_Texture_PlayerInventoryBackground";
	Panel_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
	Panel_Inven.IsFullScreen = false;
	Panel_Inven.IsTransparent = false;
	Panel_Inven.IsUseLayout = true;
	Panel_Inven.bUseNineSlice = false;

	Panel_Inven.LayoutDesc.m_Spacing = { 4.f ,4.f };
	Panel_Inven.LayoutDesc.m_Col = 6;
	Panel_Inven.LayoutDesc.m_Raw = 8;
	Panel_Inven.LayoutDesc.m_Offset = { 0.f, 0.f };


	shared_ptr<CUIPanel> InvenPanel = CUIPanel::Create(m_pDevice, m_pContext);
	InvenPanel->Initialize(&Panel_Inven);
	m_InvenPanel = InvenPanel;

	Add_Child(m_InvenPanel, L"Panel_inven", false);


	

	///// 데미지 슬로 패널 /////
	CUIPanel::UIPANEL_DESC Panel_damage = {};
	Panel_damage.TextureProtoName = L"Prototype_Component_Texture_HealthBarPanel";
	Panel_damage.TextureComLevel = ETOI(LEVEL::STATIC);
	Panel_damage.IsFullScreen = false;
	Panel_damage.IsTransparent = false;
	Panel_damage.IsUseLayout = true;

	Panel_damage.LayoutDesc.m_Spacing = { 2.f ,2.f };
	Panel_damage.LayoutDesc.m_Col = 0;
	Panel_damage.LayoutDesc.m_Raw = 3;
	Panel_damage.LayoutDesc.m_Offset = { 0.f, 0.f};

	Panel_damage.bUseNineSlice = true;

	shared_ptr<CUIPanel> DamgePanel = CUIPanel::Create(m_pDevice, m_pContext);
	DamgePanel->Initialize(&Panel_damage);

	for (_uint i = 0; i < 3; i++)
	{

		CUISlot::SLOT_DESC slot_Damage = {};
		slot_Damage.TextureProtoName = L"Prototype_Component_Texture_Slot_Damage";
		slot_Damage.TextureComLevel = ETOI(LEVEL::STATIC);
		slot_Damage.bUseDark = false;
	/*	slot_Damage.Interaction = false;*/
			shared_ptr<CUISlot> m_pInstance = CUISlot::Create(m_pDevice, m_pContext);
			m_pInstance->Initialize(&slot_Damage);

			wstring NameTag = L"SLOT_" + S2W(to_string(i));

			
			DamgePanel->Add_Layout_Child(m_pInstance, NameTag, false);
			m_pInstance->Set_Interactive(false);
			m_pInstance->Set_Zorder(2);
	}


	Add_Child(DamgePanel, L"Panel_damage", false);


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
		MSG_BOX("Failed to Created : CUI_Inventory");

	}
	return pInstance;
}

void CUI_Inventory::Free()
{
	__super::Free();
}
