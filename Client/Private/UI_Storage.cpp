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


CUI_Storage::CUI_Storage(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context) :
	CUIPanel(device, context)
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
	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_InvenStorageInitData>([this](const Evt_InvenStorageInitData& e)
		{

			if (e.invenType == EInvenType::SHOP)
			{
				if(m_shopTab == e.shopTabType)
				{
					m_inven = e.invenPtr;
					if (m_inven.lock() != nullptr)
						Rebuild_InventorySlot(m_inven.lock()->Get_W(), m_inven.lock()->Get_H(), m_inven.lock()->Get_Invenslot());

					return;
				}
				
			}
			else if (e.invenType == m_inventype)
			{
				m_inven = e.invenPtr;
				if (m_inven.lock() != nullptr)
					Rebuild_InventorySlot(m_inven.lock()->Get_W(), m_inven.lock()->Get_H(), m_inven.lock()->Get_Invenslot());
			}

		});

	return S_OK;
}


HRESULT CUI_Storage::OnInit(void* arg)
{

	StorageDesc* desc = static_cast<StorageDesc*>(arg);
	m_inventype = desc->invenType;
	m_shopTab = desc->shopTab;


	HRESULT hr = {};



	///// 창고 슬롯 패널 /////
	CUIPanel::UIPanelDesc panelInven = {};
	panelInven.textureProtoName = L"Prototype_Component_Texture_StorageInventoryBackground";
	panelInven.textureComLevel = ETOI(ELevel::STATIC);
	panelInven.isFullScreen = false;
	panelInven.isTransparent = false;
	panelInven.isUseLayout = true;
	panelInven.useNineSlice = false;

	panelInven.layoutDesc.spacing = { 4.f ,4.f };
	panelInven.layoutDesc.col = 7;
	panelInven.layoutDesc.row = 8;
	//Panel_Inven.LayoutDesc.m_Offset = { 0.f, 0.f };


	shared_ptr<CUIPanel> storagePanel = CUIPanel::Create(m_device, m_context);
	storagePanel->Initialize(&panelInven);
	m_invenPanel = storagePanel;

	Add_Child(m_invenPanel, L"Panel_storage", false);


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





	hr = __super::OnInit(desc);


	return hr;
}

void CUI_Storage::OnActive()
{
	__super::OnActive();

	Evt_OpenInventory e;
	e.inven = m_inven;
	m_gameInstance.lock()->Get_EventBus()->Publish(e);

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
    if (!m_inven.expired() && m_invenPanel && !m_slot.empty())
        MousePosToSlot();
    __super::OnUpdate(timeDelta);
}

void CUI_Storage::OnPrepareRender()
{
    auto inventory = m_inven.lock();
    if (!inventory || !m_invenPanel || m_slot.empty())
        return;
	auto& invenItem = m_inven.lock()->Get_InventoryItem();
	auto& invenSlot = m_inven.lock()->Get_Invenslot();

	for (_uint index = 0; index < invenSlot.size(); index++)
	{
		_float4 slotColor = {};
		_float4 colorReset = { 0.f,0.f,0.f,0.f };

		m_slot[index]->Set_ColorFlat(colorReset);
		switch (invenSlot[index].slotColor)
		{
		case EPlaceColor::GREEN:
			slotColor = { 0.f,1.f,0.f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_slot[index]->Set_ColorFlat(slotColor);

			break;
		case EPlaceColor::ORANGE:
			slotColor = { 1.0f,0.6f,0.0f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_slot[index]->Set_ColorFlat(slotColor);

			break;
		case EPlaceColor::RED:
			slotColor = { 1.f,0.f,0.f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_slot[index]->Set_ColorFlat(slotColor);
			break;
		case EPlaceColor::END:

			//m_Slot[index]->Set_ColorFlat(false);
			break;
		}

		if (invenSlot[index].isBroken == true)
		{
			m_slot[index]->Set_TextureIndex(1);
		}
	}


	for (auto& invnelItem : invenItem)
	{
		_float4 color = {};
		switch (invnelItem.itemType)
		{
		case EItemType::MATERIAL:
		case EItemType::EQUIP:
			color = { 0.23f, 0.23f, 0.21f, 1.f };
			break;
		case EItemType::FISH:
			color = { .7f,0.2f,0.2f,1.f };
			break;

		}


		for (_uint i = 0; i < invnelItem.curBase.size(); i++)
		{

			_uint x = invnelItem.curBase[i].dx;
			_uint y = invnelItem.curBase[i].dy;
			_uint index = y * m_inven.lock()->Get_W() + x;

			m_slot[index]->Set_UseColorFlat(true);
			m_slot[index]->Set_ColorFlat(color);
		}
	}


    Render_Item();
    __super::OnPrepareRender();
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

shared_ptr<CUI_Storage> CUI_Storage::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_Storage> instance(new CUI_Storage(device, context), [](CUI_Storage* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIStorage");

	}
	return instance;
}

void CUI_Storage::Free()
{
	__super::Free();

	m_itemUI.clear();
	m_slot.clear();

}



_bool CUI_Storage::Rebuild_InventorySlot(_uint w, _uint h, const vector<Slot>& invenSlot)
{
	//보트 슬롯모양이 바뀌면 람다등으로 Init_InventorySlot 호출 
	//find boat back ground ->  clear boat back ground's chilren -> for{for{}} add new slot children

	if (/*Inven_Contrl == nullptr||*/ m_invenPanel == nullptr)
	{
		MSG_BOX("failed  : Rebuild_InventorySlot");
		return false;
	}



	m_invenPanel->UI_Clear();


	m_invenPanel->Change_LayoutRawCol(w, h);


	/// slot ///

	m_slot.clear();
	m_slot.resize(h * w);

	for (_uint i = 0; i < h; i++)
	{
		for (_uint j = 0; j < w; j++)
		{
			Engine::CUISlot::SlotDesc slotInven = {};
			//slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
			slotInven.textureComLevel = ETOI(ELevel::STATIC);
			slotInven.scale = Vector2{ 1.6f,1.6f };
			slotInven.index = i * w + j;
			if (invenSlot[i * w + j].isLock == true)
			{
				slotInven.isTransparent = true;
			}
			slotInven.slotType = ETOI(invenSlot[i * w + j].slotType);

			switch (slotInven.slotType)
			{
			case ETOI(ESlotType::ENGINE):
				slotInven.textureProtoName = L"Prototype_Component_Texture_EngineEquipmentIcon";
				break;
			case ETOI(ESlotType::LIGHT):
				slotInven.textureProtoName = L"Prototype_Component_Texture_LightEquipmentIcon";
				break;
			case ETOI(ESlotType::NET):
				slotInven.textureProtoName = L"Prototype_Component_Texture_Slot_Inven"; // 앤 없는듯
				break;
			case ETOI(ESlotType::ROT):
				slotInven.textureProtoName = L"Prototype_Component_Texture_FishingEquipmentIcon";
				break;
			case ETOI(ESlotType::ANY):
				slotInven.textureProtoName = L"Prototype_Component_Texture_Slot_Inven";
				break;
			}


			shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_device, m_context);
			m_pInstanceINVEN->Initialize(&slotInven);

			wstring nameTag = L"SLOT_STORAGE_" + S2W(to_string(i * w + j));


			m_invenPanel->Add_Layout_Child(m_pInstanceINVEN, nameTag, false);
			m_slot[i * w + j] = m_pInstanceINVEN;

			m_pInstanceINVEN->Set_ButtonState(false);
			m_pInstanceINVEN->Set_UseDark(true);
			m_pInstanceINVEN->Set_Dark01(0.7f);

		}
	}

	m_slot.resize(h * w);
	{
		for (_uint i = 0; i < 30; i++)
		{
			CUIImage::UIImageDesc imgItemPool = {};
			imgItemPool.textureComLevel = ETOI(ELevel::STATIC);
			imgItemPool.textureProtoName = L"../Bin/Resources/Textures/Item/Fish/mackerel.png";
			imgItemPool.ZOrder = 5;
			imgItemPool.isTransparent = true;
			//ImgDesc. = true;

			shared_ptr<CUIImage> child = CUIImage::Create(m_device, m_context);
			child->Initialize(&imgItemPool);
			//pChild->UI_InActive();

			wstring nameTag = L"ITEM_STORAGE_" + S2W(to_string(i));


			m_invenPanel->Add_Child(child, nameTag, false);

			m_itemUI.push_back(child);
		}
	}

	return true;
}



_bool CUI_Storage::MousePosToSlot(/*_uint& SlotX, _uint& SlotY*/)
{
	Evt_MouseToIndexData e = {};
	e.isOnSlot = false; // 기본 false

	if (m_inven.lock()->Get_Inventype() == EInvenType::PLAYER)
	{
		e.isPlayer = true;
	}
	else
	{
		e.isPlayer = false;
	}

	LayoutDesc layout = m_invenPanel->Get_LayoutDesc();

	_float2 musPos = m_gameInstance.lock()->Get_DInput_Manger()->Get_MousePos();




	for (auto& slot : m_slot)
	{
		if (true == slot->GetUITransform()->Get_WorldRect().Contains(musPos.x, musPos.y))
		{
			//_float2 index2d = {};
			_uint index = slot->GetGridIndex();
			//SlotX =  index / layout.m_Col;
			//SlotY = index % layout.m_Col;


			e.x = index % layout.col;;
			e.y = index / layout.col;;
			e.isOnSlot = true;

			break;
		}

	}


	if (m_invenPanel->GetUITransform()->Get_WorldRect().Contains(musPos.x, musPos.y))
	{
		m_gameInstance.lock()->Get_EventBus()->Publish<Evt_MouseToIndexData>(e);
	}

	return true;

}


_float2 CUI_Storage::Calculate_RenderPos(const ItemInst& item)
{

	_float minX = FLT_MAX, minY = FLT_MAX;
	_float maxX = -FLT_MAX, maxY = -FLT_MAX;

	_uint col = m_invenPanel->Get_LayoutDesc().col;
	_uint row = m_invenPanel->Get_LayoutDesc().row;
	// 아이템이 점유한 모든 칸을 돌면서 실제 슬롯들의 위치를 수집
	for (auto& OccCell : item.curBase)
	{
		//   int index = OccCell.dy * m_w + OccCell.dx;
		//_uint col = Get_LayoutDesc().m_Col;

		//int x = OccCell.dx - 1;
		//int y = OccCell.dy - 1;
		//
		//int index = y * col + x;
		int index = OccCell.dy * col + OccCell.dx;

		_float2 slotPos = m_slot[index]->GetUITransform()->Get_AnchoredPos();

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

	auto inven = m_inven.lock();
	if (inven == nullptr)
		return;

	auto& items = inven->Get_InventoryItem();
	_float2 slotSize = m_slot[0]->GetUITransform()->Get_FinalSize();

	for (auto& UI : m_itemUI)
	{
		UI->Set_Transparent(true);
	}
	for (int i = 0; i < items.size(); i++)
	{


		LayoutDesc layout = m_invenPanel->Get_LayoutDesc();

		for (auto& UI : m_itemUI)
		{
			if (UI->Get_Transparent() == true)
			{
				ItemDef def = CItemDB::GetInstance()->GetItemByID(items[i].itemDefID);
				UI->Change_Texture(items[i].cachingTexture);
				if (items[i].cachingTexture == nullptr)
				{
					MSG_BOX("Texture NULL");
					continue;
				}
				_float2 anchoredPos = Calculate_RenderPos(items[i]);

				UI->GetUITransform()->SetAnchoredPos(anchoredPos);
				UI->GetUITransform()->SetSizeDelta(
					{ slotSize.x * def.itemShape.width + layout.spacing.x * (def.itemShape.width - 1),
					slotSize.y * def.itemShape.height + layout.spacing.y * (def.itemShape.height - 1)
					});
				// 회전
				UI->GetUITransform()->SetRotation(items[i].rotation * -90.f);
				UI->Set_Transparent(false);
				UI->GetUITransform()->SetPivot({ 0.5f, 0.5f });
				break;
			}
		}
	}

}

void CUI_Storage::Set_SlotNum(_uint col, _uint row)
{
	Rebuild_InventorySlot(col, row, m_inven.lock()->Get_Invenslot());
}


