#include "UI_Inventory.h"
#include "UIImage.h"
#include "UISlot.h"
#include "Inventory_Controller.h"
#include "EventBus.h"

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
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<EvtControllerPoiner>([this](const EvtControllerPoiner& e) {this->Inven_Contrl = e.m_contrl_Pointer; });

	///


	return CUIPanel::Initialize_Prototype();
}

HRESULT CUI_Inventory::Init_InventorySlot()
{
	_uint w, h = {};

	const vector<Slot>& m_inven_slot = Inven_Contrl->Get_Invanslot(w, h);


	///// 인밴 슬롯 패널 /////
	CUIPanel::UIPANEL_DESC Panel_Inven = {};
	Panel_Inven.TextureProtoName = L"Prototype_Component_Texture_PlayerInventoryBackground";
	Panel_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
	Panel_Inven.IsFullScreen = false;
	Panel_Inven.IsTrnasparent = false;
	Panel_Inven.IsUseLayout = true;
	Panel_Inven.bUseNineSlice = false;

	Panel_Inven.LayoutDesc.m_Spacing = { 4.f ,4.f };
	Panel_Inven.LayoutDesc.m_Col = h;
	Panel_Inven.LayoutDesc.m_Raw = w;
	Panel_Inven.LayoutDesc.m_Offset = { 0.f, 0.f };


	shared_ptr<CUIPanel> InvenPanel = CUIPanel::Create(m_pDevice, m_pContext);
	InvenPanel->Initialize(&Panel_Inven);


	/// slot ///


	for(_uint i =0; i < h; i++)
	{
		for (_uint j = 0; j < w; j++)
		{
			//if(m_inven_slot[i*w+j].IsLock == true)
			//{
			//}
			CUISlot::SLOT_DESC slot_Inven = {};
			slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
			slot_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
			slot_Inven.vScale = Vector2{ 1.6f,1.6f };


			shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_pDevice, m_pContext);
			m_pInstanceINVEN->Initialize(&slot_Inven);

			wstring NameTag = L"SLOT_INVEN_" + S2W(to_string(i));


			InvenPanel->Add_Layout_Child(m_pInstanceINVEN, NameTag, false);
		}
	}


	//for (_uint i = 0; i < Panel_Inven.LayoutDesc.m_Col * Panel_Inven.LayoutDesc.m_Raw; i++)
	//{
	//
	//	CUISlot::SLOT_DESC slot_Inven = {};
	//	slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
	//	slot_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
	//	slot_Inven.vScale = Vector2{ 1.6f,1.6f };
	//
	//
	//	shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_pDevice, m_pContext);
	//	m_pInstanceINVEN->Initialize(&slot_Inven);
	//
	//	wstring NameTag = L"SLOT_INVEN_" + S2W(to_string(i));
	//
	//
	//	InvenPanel->Add_Layout_Child(m_pInstanceINVEN, NameTag, false);
	//}
	Add_Child(InvenPanel, L"Panel_inven", false);

	return S_OK;
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


	Init_InventorySlot();
	

	///// 데미지 슬로 패널 /////
	CUIPanel::UIPANEL_DESC Panel_damage = {};
	Panel_damage.TextureProtoName = L"Prototype_Component_Texture_HealthBarPanel";
	Panel_damage.TextureComLevel = ETOI(LEVEL::STATIC);
	Panel_damage.IsFullScreen = false;
	Panel_damage.IsTrnasparent = false;
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
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_Inventory::Free()
{
	__super::Free();
}
