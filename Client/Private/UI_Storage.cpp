#include "UI_Storage.h"
#include "UISlot.h"

#include "UIText.h"

#include "UIImage.h"

#include "Inventory_Controller.h"
#include "EventBus.h"
#include "Client_Enum.h"
#include "ItemDB.h"
#include "GameInstance.h"
#include "DInput_Manager.h"


CUI_Storage::CUI_Storage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CUIPanel(pDevice, pContext)
{
}

CUI_Storage::CUI_Storage(const CUIPanel& prototype) :
	CUIPanel(prototype)
{
}
HRESULT CUI_Storage::Initialize_Prototype()
{
	// 이건 그냥  PreInitialize 대용으로 사용
	//
	///발행이 나중에 되야함 
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_InvenStrageInit_Data>([this](const Evt_InvenStrageInit_Data& e)
		{

			if (e.inventype == INVENTYPE::SHOP)
			{
				if(m_shopTab == e.ShopTabType)
				{
					m_Inven = e.Inven_ptr;
					if (m_Inven.lock() != nullptr)
						Rebuild_InventorySlot(m_Inven.lock()->Get_W(), m_Inven.lock()->Get_H(), m_Inven.lock()->Get_Invenslot());

					return;
				}
				
			}
			else if (e.inventype == m_inventype)
			{
				m_Inven = e.Inven_ptr;
				if (m_Inven.lock() != nullptr)
					Rebuild_InventorySlot(m_Inven.lock()->Get_W(), m_Inven.lock()->Get_H(), m_Inven.lock()->Get_Invenslot());
			}

		});

	return S_OK;
}


HRESULT CUI_Storage::OnInit(void* pArg)
{

	STORAGE_DESC* pDesc = static_cast<STORAGE_DESC*>(pArg);
	m_inventype = pDesc->Inventype;
	m_shopTab = pDesc->shopTab;


	HRESULT hr = {};



	///// 창고 슬롯 패널 /////
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
	//m_Slot.resize(Panel_Inven.LayoutDesc.m_Col * Panel_Inven.LayoutDesc.m_Row);
	//for (_uint i = 0; i < Panel_Inven.LayoutDesc.m_Col* Panel_Inven.LayoutDesc.m_Row; i++)
	//{
	//	/*for (_uint j = 0; j < w; j++)
	//	{*/
	//		CUISlot::SLOT_DESC slot_Inven = {};
	//		slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
	//		slot_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
	//		slot_Inven.vScale = Vector2{ 1.6f,1.6f };

	//		slot_Inven.slotType = ETOI(SLOT_TYPE::ANY);

	//		shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_pDevice, m_pContext);
	//		m_pInstanceINVEN->Initialize(&slot_Inven);

	//		wstring NameTag = L"SLOT_STORAGE_" + S2W(to_string(i));
	//		m_Slot[i] = m_pInstanceINVEN;

	//		m_InvenPanel->Add_Layout_Child(m_pInstanceINVEN, NameTag, false);
	//	//}
	//}





	hr = __super::OnInit(pDesc);


	return hr;
}

void CUI_Storage::OnActive()
{
	__super::OnActive();

	Evt_OpenInventory e;
	e.inven = m_Inven;
	m_pGameInstance.lock()->Get_EventBus()->Publish(e);

	//m_shopTab = SHOPTAB::ENGINE;
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

	MousePosToSlot();


	auto& InvenItem = m_Inven.lock()->Get_InventoryItem();
	auto& InvenSlot = m_Inven.lock()->Get_Invenslot();

	for (_uint index = 0; index < InvenSlot.size(); index++)
	{
		_float4 Muscolor = {};
		_float4 colorReset = { 0.f,0.f,0.f,0.f };

		m_Slot[index]->Set_ColorFlat(colorReset);
		switch (InvenSlot[index].Slot_Color)
		{
		case PLACE_COLOR::GREEN:
			Muscolor = { 0.f,1.f,0.f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_Slot[index]->Set_ColorFlat(Muscolor);

			break;
		case PLACE_COLOR::ORANGE:
			Muscolor = { 1.0f,0.6f,0.0f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_Slot[index]->Set_ColorFlat(Muscolor);

			break;
		case PLACE_COLOR::RED:
			Muscolor = { 1.f,0.f,0.f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_Slot[index]->Set_ColorFlat(Muscolor);
			break;
		case PLACE_COLOR::END:

			//m_Slot[index]->Set_ColorFlat(false);
			break;
		}

		if (InvenSlot[index].IsBroken == true)
		{
			m_Slot[index]->Set_TextureIndex(1);
		}
	}


	for (auto& invnelItem : InvenItem)
	{
		_float4 color = {};
		switch (invnelItem.ItemType)
		{
		case ITEM_TYPE::MATERIAL:
		case ITEM_TYPE::EQUIP:
			color = { 0.23f, 0.23f, 0.21f, 1.f };
			break;
		case ITEM_TYPE::FISH:
			color = { .7f,0.2f,0.2f,1.f };
			break;

		}


		for (_uint i = 0; i < invnelItem.CurBase.size(); i++)
		{

			_uint x = invnelItem.CurBase[i].dx;
			_uint y = invnelItem.CurBase[i].dy;
			_uint index = y * m_Inven.lock()->Get_W() + x;

			m_Slot[index]->Set_UseColorFlat(true);
			m_Slot[index]->Set_ColorFlat(color);
		}
	}


	__super::OnUpdate(timeDelta);

	Render_Item();



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

	m_ItemUI.clear();
	m_Slot.clear();

}



_bool CUI_Storage::Rebuild_InventorySlot(_uint w, _uint h, const vector<Slot>& inven_slot)
{
	//보트 슬롯모양이 바뀌면 람다등으로 Init_InventorySlot 호출 
	//find boat back ground ->  clear boat back ground's chilren -> for{for{}} add new slot children

	if (/*Inven_Contrl == nullptr||*/ m_InvenPanel == nullptr)
	{
		MSG_BOX("failed  : Rebuild_InventorySlot");
		return false;
	}



	m_InvenPanel->UI_Clear();


	m_InvenPanel->Change_LayoutRawCol(w, h);


	/// slot ///

	m_Slot.clear();
	m_Slot.resize(h * w);

	for (_uint i = 0; i < h; i++)
	{
		for (_uint j = 0; j < w; j++)
		{
			Engine::CUISlot::SLOT_DESC slot_Inven = {};
			//slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
			slot_Inven.TextureComLevel = ETOI(LEVEL::STATIC);
			slot_Inven.vScale = Vector2{ 1.6f,1.6f };
			slot_Inven.Index = i * w + j;
			if (inven_slot[i * w + j].IsLock == true)
			{
				slot_Inven.IsTransparent = true;
			}
			slot_Inven.slotType = ETOI(inven_slot[i * w + j].slotType);

			switch (slot_Inven.slotType)
			{
			case ETOI(SLOT_TYPE::ENGINE):
				slot_Inven.TextureProtoName = L"Prototype_Component_Texture_EngineEquipmentIcon";
				break;
			case ETOI(SLOT_TYPE::LIGHT):
				slot_Inven.TextureProtoName = L"Prototype_Component_Texture_LightEquipmentIcon";
				break;
			case ETOI(SLOT_TYPE::NET):
				slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven"; // 앤 없는듯
				break;
			case ETOI(SLOT_TYPE::ROT):
				slot_Inven.TextureProtoName = L"Prototype_Component_Texture_FishingEquipmentIcon";
				break;
			case ETOI(SLOT_TYPE::ANY):
				slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
				break;
			}


			shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_pDevice, m_pContext);
			m_pInstanceINVEN->Initialize(&slot_Inven);

			wstring NameTag = L"SLOT_STORAGE_" + S2W(to_string(i * w + j));


			m_InvenPanel->Add_Layout_Child(m_pInstanceINVEN, NameTag, false);
			m_Slot[i * w + j] = m_pInstanceINVEN;

			m_pInstanceINVEN->Set_ButtonState(false);
			m_pInstanceINVEN->Set_UseDark(true);
			m_pInstanceINVEN->Set_Dark01(0.7f);

		}
	}

	m_Slot.resize(h * w);
	{
		for (_uint i = 0; i < 30; i++)
		{
			CUIImage::UIIMAGE_DESC ImgItemPool = {};
			ImgItemPool.TextureComLevel = ETOI(LEVEL::STATIC);
			ImgItemPool.TextureProtoName = L"../Bin/Resources/Textures/Item/Fish/mackerel.png";
			ImgItemPool.ZOrder = 5;
			ImgItemPool.IsTransparent = true;
			//ImgDesc. = true;

			shared_ptr<CUIImage> pChild = CUIImage::Create(m_pDevice, m_pContext);
			pChild->Initialize(&ImgItemPool);
			//pChild->UI_InActive();

			wstring NameTag = L"ITEM_STORAGE_" + S2W(to_string(i));


			m_InvenPanel->Add_Child(pChild, NameTag, false);

			m_ItemUI.push_back(pChild);
		}
	}

	return true;
}



_bool CUI_Storage::MousePosToSlot(/*_uint& SlotX, _uint& SlotY*/)
{
	Evt_MouseToIndex_Data e = {};
	e.IsOnSlot = false; // 기본 false

	if (m_Inven.lock()->Get_Inventype() == INVENTYPE::PLAYER)
	{
		e.IsPlayer = true;
	}
	else
	{
		e.IsPlayer = false;
	}

	LAYOUT_DESC layout = m_InvenPanel->Get_LayoutDesc();

	_float2 musPos = m_pGameInstance.lock()->Get_DInput_Manger()->Get_MousePos();




	for (auto& slot : m_Slot)
	{
		if (true == slot->GetUITransform()->Get_WorldRect().Contains(musPos.x, musPos.y))
		{
			//_float2 index2d = {};
			_uint index = slot->GetGridIndex();
			//SlotX =  index / layout.m_Col;
			//SlotY = index % layout.m_Col;


			e.x = index % layout.m_Col;;
			e.y = index / layout.m_Col;;
			e.IsOnSlot = true;

			break;
		}

	}


	if (m_InvenPanel->GetUITransform()->Get_WorldRect().Contains(musPos.x, musPos.y))
	{
		m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_MouseToIndex_Data>(e);
	}

	return true;

}


_float2 CUI_Storage::Calculate_RenderPos(const Item_Inst& item)
{

	_float minX = FLT_MAX, minY = FLT_MAX;
	_float maxX = -FLT_MAX, maxY = -FLT_MAX;

	_uint col = m_InvenPanel->Get_LayoutDesc().m_Col;
	_uint row = m_InvenPanel->Get_LayoutDesc().m_Row;
	// 아이템이 점유한 모든 칸을 돌면서 실제 슬롯들의 위치를 수집
	for (auto& OccCell : item.CurBase)
	{
		//   int index = OccCell.dy * m_w + OccCell.dx;
		//_uint col = Get_LayoutDesc().m_Col;

		//int x = OccCell.dx - 1;
		//int y = OccCell.dy - 1;
		//
		//int index = y * col + x;
		int index = OccCell.dy * col + OccCell.dx;

		_float2 slotPos = m_Slot[index]->GetUITransform()->Get_AnchoredPos();

		minX = min(minX, slotPos.x);
		maxX = max(maxX, slotPos.x);
		minY = min(minY, slotPos.y);
		maxY = max(maxY, slotPos.y);




	}

	// 4. 수집된 슬롯 좌표들의 정중앙을 구함
	_float2 center = {
		(minX + maxX) * 0.5f,
		(minY + maxY) * 0.5f
	};


	return center;

}




void CUI_Storage::Render_Item()
{


	//이 함수는 유아이가 추가되거나 줄었을떄 신호받으면 그떄 한번씩 하는거임

	auto inven = m_Inven.lock();
	if (inven == nullptr)
		return;

	auto& items = inven->Get_InventoryItem();
	_float2 slotSize = m_Slot[0]->GetUITransform()->Get_FinalSize();

	for (auto& UI : m_ItemUI)
	{
		UI->Set_Transparent(true);
	}
	for (int i = 0; i < items.size(); i++)
	{


		LAYOUT_DESC layout = m_InvenPanel->Get_LayoutDesc();

		for (auto& UI : m_ItemUI)
		{
			if (UI->Get_Transparent() == true)
			{
				Item_Def def = CItemDB::GetInstance()->GetItemByID(items[i].ItemDef_ID);
				UI->Change_Texture(items[i].pCashingTexture);
				if (items[i].pCashingTexture == nullptr)
				{
					MSG_BOX("Texture NULL");
					continue;
				}
				_float2 vAnchoredPos = Calculate_RenderPos(items[i]);

				UI->GetUITransform()->SetAnchoredPos(vAnchoredPos);
				UI->GetUITransform()->SetSizeDelta(
					{ slotSize.x * def.ItemShape.Width + layout.m_Spacing.x * (def.ItemShape.Width - 1),
					slotSize.y * def.ItemShape.Height + layout.m_Spacing.y * (def.ItemShape.Height - 1)
					});
				// 회전
				UI->GetUITransform()->SetRotation(items[i].Rotation * -90.f);
				UI->Set_Transparent(false);
				UI->GetUITransform()->SetPivot({ 0.5f, 0.5f });
				break;
			}
		}
	}

}

void CUI_Storage::Set_SlotNum(_uint col, _uint row)
{
	Rebuild_InventorySlot(col, row, m_Inven.lock()->Get_Invenslot());
}


