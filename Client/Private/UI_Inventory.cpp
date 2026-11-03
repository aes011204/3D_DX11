#include "UI_Inventory.h"

#include "UIText.h"

#include "UIImage.h"
#include "UISlot.h"
#include "Inventory_Controller.h"
#include "EventBus.h"
#include "Client_Enum.h"
#include "ItemDB.h"
#include "GameInstance.h"
#include "DInput_Manager.h"

namespace Engine
{
	struct Evt_Damage;
}

CUI_Inventory::CUI_Inventory(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CUIPanel(device, context)
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
	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_InvenPlayerInitData>([this](const Evt_InvenPlayerInitData& e)
		{
			m_inven = e.invenPtr;
			if (m_inven.lock() != nullptr)
				Rebuild_InventorySlot(m_inven.lock()->Get_W(), m_inven.lock()->Get_H(), m_inven.lock()->Get_Invenslot());
		});

	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_ShipStat>([this](const Evt_ShipStat& e)
		{
			wstring info = format(L"어선 속도 : {}kn\n낚시 속도 : {}%\n등불 : {}lm",
				e.engineSpeed, e.fishingSpeed, e.lightIntensity);
			this->m_textInfo->Set_Text(info);

			wstring seaTypes = L"";
			_uint count = 0;
			for (_uint i = 0; i < (_uint)ESeaType::END; ++i)
			{
				// e.mask에 해당 비트가 켜져 있는지 확인 (예: 1 << 0, 1 << 1 ...)
				if (e.seaMask & (1 << i))
				{
					if (count > 0) seaTypes += L", "; // 두 번째 이름부터는 쉼표 추가
					seaTypes += GetSeaTypeName((ESeaType)(1 << i));
					count++;
				}
			}

			if (count == 0) seaTypes = L"없음";

			wstring info2 = format(L"잡을수 있는 어종 :\n {}", seaTypes);
			this->m_textInfo1->Set_Text(info2);
		});

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_Damage>(
		[this](const Evt_Damage& e)
		{
			if (e.damageCount > m_damageSlot.size())
				return;
			
			for (_uint i = 0; i < e.damageCount; i++)
			{
				m_damageSlot[i]->Set_TextureIndex(1);
			}
		});

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_FixAllDone>(
		[this](const Evt_FixAllDone& e)
		{
			
			for (_uint i = 0; i < m_damageSlot.size(); i++)
			{
				m_damageSlot[i]->Set_TextureIndex(0);
			}
		});

	return CUIPanel::Initialize_Prototype();
}

_bool CUI_Inventory::Rebuild_InventorySlot(_uint w, _uint h, const vector<Slot>& invenSlot)
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

			wstring nameTag = L"SLOT_INVEN_" + S2W(to_string(i * w + j));


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

			wstring nameTag = L"ITEM_" + S2W(to_string(i));


			m_invenPanel->Add_Child(child, nameTag, false);

			m_itemUI.push_back(child);
		}
	}

	return true;
}

HRESULT CUI_Inventory::OnInit(void* arg)
{

	InventoryDesc* desc = static_cast<InventoryDesc*>(arg);
	HRESULT hr = {};



	///// 정보 이미지 /////

	CUIImage::UIImageDesc imgDesc = {};
	imgDesc.textureComLevel = ETOI(ELevel::STATIC);
	imgDesc.textureProtoName = L"Prototype_Component_Texture_PanelBackground";
	imgDesc.ZOrder = 3;

	shared_ptr<CUIImage> child = CUIImage::Create(m_device, m_context);
	child->Initialize(&imgDesc);

	wstring nameTag = L"INVEN_BACK";

	{
		CUIText::TextDesc infoTexDesc = {};
		infoTexDesc.fontTag = L"Noto_Sans_CJK_SC";
		infoTexDesc.text = format(L"어선 속도 : {}kn\n낚시 속도 : {}%\n등불 : {}lm",
			0, 0, 0);
		infoTexDesc.fontAlign = CUIText::EFontAlign::LEFT;
		shared_ptr<CUIText>infoTex = CUIText::Create(m_device, m_context);
		infoTex->Initialize(&infoTexDesc);
		child->Add_Child(infoTex, L"InfoTex", false);
		m_textInfo = infoTex;


		CUIText::TextDesc infoTexDesc1 = {};
		infoTexDesc1.fontTag = L"Noto_Sans_CJK_SC";
		infoTexDesc1.text = format(L"잡을수 있는 어종 :\n {}kn",0);
		infoTexDesc1.fontAlign = CUIText::EFontAlign::LEFT;
		shared_ptr<CUIText> infoTex1 = CUIText::Create(m_device, m_context);
		infoTex1->Initialize(&infoTexDesc1);
		child->Add_Child(infoTex1, L"InfoTex_1", false);
		m_textInfo1 = infoTex1;

	}

	Add_Child(child, nameTag, false);


	///// 인밴 슬롯 패널 /////
	CUIPanel::UIPanelDesc panelInven = {};
	panelInven.textureProtoName = L"Prototype_Component_Texture_PlayerInventoryBackground";
	panelInven.textureComLevel = ETOI(ELevel::STATIC);
	panelInven.isFullScreen = false;
	panelInven.isTransparent = false;
	panelInven.isUseLayout = true;
	panelInven.useNineSlice = false;

	panelInven.layoutDesc.spacing = { 4.f ,4.f };
	panelInven.layoutDesc.col = 6;
	panelInven.layoutDesc.row = 8;
	//Panel_Inven.LayoutDesc.m_Offset = { 0.f, 0.f };


	shared_ptr<CUIPanel> invenPanel = CUIPanel::Create(m_device, m_context);
	invenPanel->Initialize(&panelInven);
	m_invenPanel = invenPanel;

	Add_Child(m_invenPanel, L"Panel_inven", false);



	///// 데미지 슬로 패널 /////
	CUIPanel::UIPanelDesc panelDamage = {};
	panelDamage.textureProtoName = L"Prototype_Component_Texture_HealthBarPanel";
	panelDamage.textureComLevel = ETOI(ELevel::STATIC);
	panelDamage.isFullScreen = false;
	panelDamage.isTransparent = false;
	panelDamage.isUseLayout = true;

	panelDamage.layoutDesc.spacing = { 2.f ,2.f };
	panelDamage.layoutDesc.col = 0;
	panelDamage.layoutDesc.row = 3;
	//Panel_damage.LayoutDesc.m_Offset = { 0.f, 0.f};

	panelDamage.useNineSlice = true;

	shared_ptr<CUIPanel> damagePanel = CUIPanel::Create(m_device, m_context);
	damagePanel->Initialize(&panelDamage);

	for (_uint i = 0; i < 3; i++)
	{

		CUISlot::SlotDesc slotDamage = {};
		slotDamage.textureProtoName = L"Prototype_Component_Texture_Slot_Damage";
		slotDamage.textureComLevel = ETOI(ELevel::STATIC);
		slotDamage.useDark = false;
		/*	slot_Damage.Interaction = false;*/
		shared_ptr<CUISlot> m_pInstance = CUISlot::Create(m_device, m_context);
		m_pInstance->Initialize(&slotDamage);

		wstring nameTag = L"SLOT_" + S2W(to_string(i));


		damagePanel->Add_Layout_Child(m_pInstance, nameTag, false);
		m_pInstance->Set_Interactive(false);
		m_pInstance->Set_Zorder(2);
		m_damageSlot.push_back(m_pInstance);
	}


	Add_Child(damagePanel, L"Panel_damage", false);


	{

	}



	hr = __super::OnInit(desc);
	return hr;
}

void CUI_Inventory::OnActive()
{
	__super::OnActive();

}

void CUI_Inventory::OnInActive()
{

	Evt_MouseToIndexData e = {};
	e.isOnSlot = false; // 기본 false

	//if (m_Inven.lock()->Get_Inventype() == INVENTYPE::PLAYER)
	//{
		e.isPlayer = true;
	//}
	//else
	//{
	//	e.IsPlayer = false;
	//}

	


	

	e.x = 0;
		e.y = 0;
			e.isOnSlot = false;



		m_gameInstance.lock()->Get_EventBus()->Publish<Evt_MouseToIndexData>(e);
	

	__super::OnInActive();

}

void CUI_Inventory::OnDisabled()
{
	__super::OnDisabled();

}

void CUI_Inventory::OnUpdate(const _float& timeDelta)
{
    if (!m_inven.expired() && m_invenPanel && !m_slot.empty())
        MousePosToSlot();
    __super::OnUpdate(timeDelta);
}

void CUI_Inventory::OnPrepareRender()
{
    auto inventory = m_inven.lock();
    if (!inventory || !m_invenPanel || m_slot.empty())
        return;
	auto& invenItem = m_inven.lock()->Get_InventoryItem();
	auto& invenSlot = m_inven.lock()->Get_Invenslot();

	for (_uint index = 0; index < invenSlot.size(); index++)
	{
		_float4 slotColor={};
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


		if(invenSlot[index].isLock == false)
		{
			if (invenSlot[index].isBroken == true)
			{
				m_slot[index]->Set_TextureIndex(1);
			}
			else
			{
				m_slot[index]->Set_TextureIndex(0);
			}
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

_bool CUI_Inventory::MousePosToSlot(/*_uint& SlotX, _uint& SlotY*/)
{
	Evt_MouseToIndexData e = {};
	e.isOnSlot = false; // 기본 false

	if(m_inven.lock()->Get_Inventype() == EInvenType::PLAYER)
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
		//else
		//{
		//	
		//	e.IsOnSlot = false;
		//	//m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_MouseToIndex_Data>(e);

		//}

	}
	//return false;


	//LOG_F(LOG_LEVEL::INFO, "x: %d ,y: %d ", e.x, e.y);

	if (m_invenPanel->GetUITransform()->Get_WorldRect().Contains(musPos.x, musPos.y))
	{
		m_gameInstance.lock()->Get_EventBus()->Publish<Evt_MouseToIndexData>(e);
	}
	
	return true;

}


//_float2 CUI_Inventory::SlotToPos(int col, int row)
//{
//	LAYOUT_DESC layout = m_InvenPanel->Get_LayoutDesc();
//
//	_float2 slotSize = { layout.m_SlotSize, layout.m_SlotSize };
//	_float2 spacing = layout.m_Spacing;
//
//	float totalWidth = (layout.m_Col * slotSize.x) +
//		((layout.m_Col - 1) * spacing.x);
//
//	float totalHeight = (layout.m_Row * slotSize.y) +
//		((layout.m_Row - 1) * spacing.y);
//
//	_float2 startPos = {
//		-(totalWidth / 2.f) + (slotSize.x / 2.f) + layout.m_Offset.x,
//		(totalHeight / 2.f) - (slotSize.y / 2.f) + layout.m_Offset.y
//	};
//
//	float posX = startPos.x + (slotSize.x + spacing.x) * col;
//	float posY = startPos.y - (slotSize.y + spacing.y) * row;
//
//	return { posX, posY };
//}
_float2 CUI_Inventory::Calculate_RenderPos(const ItemInst& item)
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
	////_float2 size = Get_LayoutDesc().m_SlotSize;
	//center.x += (m_InvenPanel->Get_LayoutDesc().m_SlotSize) * 0.5f;
	//center.y -= m_InvenPanel->Get_LayoutDesc().m_SlotSize * 0.5f;

	return center;
	//	LAYOUT_DESC layout = m_InvenPanel->Get_LayoutDesc();
	//
	//	_float2 slotSize = m_Slot[0]->GetUITransform()->Get_FinalSize();
	//
	//	float totalWidth = (layout.m_Col * slotSize.x) +
	//		((layout.m_Col - 1) * layout.m_Spacing.x);
	//
	//	float totalHeight = (layout.m_Row * slotSize.y) +
	//		((layout.m_Row - 1) * layout.m_Spacing.y);
	//
	//	_float2 startPos = {
	//		-(totalWidth / 2.f) + (slotSize.x / 2.f) + layout.m_Offset.x,
	//		(totalHeight / 2.f) - (slotSize.y / 2.f) + layout.m_Offset.y
	//	};
	//
	//	float minX = FLT_MAX, minY = FLT_MAX;
	//	float maxX = -FLT_MAX, maxY = -FLT_MAX;
	//
	//	for (auto& OccCell : item.CurBase)
	//	{
	//		_uint col = Get_LayoutDesc().m_Col;
	//		int index = OccCell.dy * col + OccCell.dx;
	//
	//		_float2 slotPos = m_Slot[index]->GetUITransform()->Get_AnchoredPos();
	//
	//		minX = min(minX, slotPos.x);
	//		maxX = max(maxX, slotPos.x);
	//		minY = min(minY, slotPos.y);
	//		maxY = max(maxY, slotPos.y);
	//	}
	//
	//	_float2 center = {
	//	(minX + maxX) * 0.5f,
	//	(minY + maxY) * 0.5f
	//	};
	//
	//	
	//	return center;
}




void CUI_Inventory::Render_Item()
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


shared_ptr<CUI_Inventory> CUI_Inventory::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_Inventory> instance(new CUI_Inventory(device, context), [](CUI_Inventory* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Inventory");

	}
	return instance;
}

void CUI_Inventory::Free()
{
	__super::Free();
	m_itemUI.clear();
	m_slot.clear();

}
