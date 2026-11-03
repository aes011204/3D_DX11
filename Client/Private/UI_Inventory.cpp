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
	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_InvenPlayerInitData>([this](const Evt_InvenPlayerInitData& e)
		{
			m_inven = e.invenPtr;
			if (m_inven.lock() != nullptr)
				Rebuild_InventorySlot(m_inven.lock()->Get_W(), m_inven.lock()->Get_H(), m_inven.lock()->Get_Invenslot());
		});

	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_ShipStat>([this](const Evt_ShipStat& e)
		{
			wstring strInfo = format(L"어선 속도 : {}kn\n낚시 속도 : {}%\n등불 : {}lm",
				e.engineSpeed, e.fishingSpeed, e.lightIntensity);
			this->m_textInfo->Set_Text(strInfo);

			wstring strSeaTypes = L"";
			_uint iCount = 0;
			for (_uint i = 0; i < (_uint)ESeaType::END; ++i)
			{
				// e.mask에 해당 비트가 켜져 있는지 확인 (예: 1 << 0, 1 << 1 ...)
				if (e.seaMask & (1 << i))
				{
					if (iCount > 0) strSeaTypes += L", "; // 두 번째 이름부터는 쉼표 추가
					strSeaTypes += GetSeaTypeName((ESeaType)(1 << i));
					iCount++;
				}
			}

			if (iCount == 0) strSeaTypes = L"없음";

			wstring strInfo_2 = format(L"잡을수 있는 어종 :\n {}", strSeaTypes);
			this->m_textInfo1->Set_Text(strInfo_2);
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

_bool CUI_Inventory::Rebuild_InventorySlot(_uint w, _uint h, const vector<Slot>& inven_slot)
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
			Engine::CUISlot::SlotDesc slot_Inven = {};
			//slot_Inven.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
			slot_Inven.textureComLevel = ETOI(ELevel::STATIC);
			slot_Inven.scale = Vector2{ 1.6f,1.6f };
			slot_Inven.index = i * w + j;
			if (inven_slot[i * w + j].isLock == true)
			{
				slot_Inven.isTransparent = true;
			}
			slot_Inven.slotType = ETOI(inven_slot[i * w + j].slotType);


			switch (slot_Inven.slotType)
			{
			case ETOI(ESlotType::ENGINE):
				slot_Inven.textureProtoName = L"Prototype_Component_Texture_EngineEquipmentIcon";
				break;
			case ETOI(ESlotType::LIGHT):
				slot_Inven.textureProtoName = L"Prototype_Component_Texture_LightEquipmentIcon";
				break;
			case ETOI(ESlotType::NET):
				slot_Inven.textureProtoName = L"Prototype_Component_Texture_Slot_Inven"; // 앤 없는듯
				break;
			case ETOI(ESlotType::ROT):
				slot_Inven.textureProtoName = L"Prototype_Component_Texture_FishingEquipmentIcon";
				break;
			case ETOI(ESlotType::ANY):
				slot_Inven.textureProtoName = L"Prototype_Component_Texture_Slot_Inven";
				break;
			}


			shared_ptr<CUISlot> m_pInstanceINVEN = CUISlot::Create(m_device, m_context);
			m_pInstanceINVEN->Initialize(&slot_Inven);

			wstring NameTag = L"SLOT_INVEN_" + S2W(to_string(i * w + j));


			m_invenPanel->Add_Layout_Child(m_pInstanceINVEN, NameTag, false);
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
			CUIImage::UIImageDesc ImgItemPool = {};
			ImgItemPool.textureComLevel = ETOI(ELevel::STATIC);
			ImgItemPool.textureProtoName = L"../Bin/Resources/Textures/Item/Fish/mackerel.png";
			ImgItemPool.ZOrder = 5;
			ImgItemPool.isTransparent = true;
			//ImgDesc. = true;

			shared_ptr<CUIImage> pChild = CUIImage::Create(m_device, m_context);
			pChild->Initialize(&ImgItemPool);
			//pChild->UI_InActive();

			wstring NameTag = L"ITEM_" + S2W(to_string(i));


			m_invenPanel->Add_Child(pChild, NameTag, false);

			m_itemUi.push_back(pChild);
		}
	}

	return true;
}

HRESULT CUI_Inventory::OnInit(void* pArg)
{

	InventoryDesc* pDesc = static_cast<InventoryDesc*>(pArg);
	HRESULT hr = {};



	///// 정보 이미지 /////

	CUIImage::UIImageDesc ImgDesc = {};
	ImgDesc.textureComLevel = ETOI(ELevel::STATIC);
	ImgDesc.textureProtoName = L"Prototype_Component_Texture_PanelBackground";
	ImgDesc.ZOrder = 3;

	shared_ptr<CUIImage> pChild = CUIImage::Create(m_device, m_context);
	pChild->Initialize(&ImgDesc);

	wstring NameTag = L"INVEN_BACK";

	{
		CUIText::TextDesc InfoTexDesc = {};
		InfoTexDesc.fontTag = L"Noto_Sans_CJK_SC";
		InfoTexDesc.text = format(L"어선 속도 : {}kn\n낚시 속도 : {}%\n등불 : {}lm",
			0, 0, 0);
		InfoTexDesc.fontAlign = CUIText::EFontAlign::LEFT;
		shared_ptr<CUIText>InfoTex = CUIText::Create(m_device, m_context);
		InfoTex->Initialize(&InfoTexDesc);
		pChild->Add_Child(InfoTex, L"InfoTex", false);
		m_textInfo = InfoTex;


		CUIText::TextDesc InfoTexDesc_1 = {};
		InfoTexDesc_1.fontTag = L"Noto_Sans_CJK_SC";
		InfoTexDesc_1.text = format(L"잡을수 있는 어종 :\n {}kn",0);
		InfoTexDesc_1.fontAlign = CUIText::EFontAlign::LEFT;
		shared_ptr<CUIText> InfoTex_1 = CUIText::Create(m_device, m_context);
		InfoTex_1->Initialize(&InfoTexDesc_1);
		pChild->Add_Child(InfoTex_1, L"InfoTex_1", false);
		m_textInfo1 = InfoTex_1;

	}

	Add_Child(pChild, NameTag, false);


	///// 인밴 슬롯 패널 /////
	CUIPanel::UIPanelDesc Panel_Inven = {};
	Panel_Inven.textureProtoName = L"Prototype_Component_Texture_PlayerInventoryBackground";
	Panel_Inven.textureComLevel = ETOI(ELevel::STATIC);
	Panel_Inven.isFullScreen = false;
	Panel_Inven.isTransparent = false;
	Panel_Inven.isUseLayout = true;
	Panel_Inven.useNineSlice = false;

	Panel_Inven.layoutDesc.spacing = { 4.f ,4.f };
	Panel_Inven.layoutDesc.col = 6;
	Panel_Inven.layoutDesc.row = 8;
	//Panel_Inven.LayoutDesc.m_Offset = { 0.f, 0.f };


	shared_ptr<CUIPanel> InvenPanel = CUIPanel::Create(m_device, m_context);
	InvenPanel->Initialize(&Panel_Inven);
	m_invenPanel = InvenPanel;

	Add_Child(m_invenPanel, L"Panel_inven", false);



	///// 데미지 슬로 패널 /////
	CUIPanel::UIPanelDesc Panel_damage = {};
	Panel_damage.textureProtoName = L"Prototype_Component_Texture_HealthBarPanel";
	Panel_damage.textureComLevel = ETOI(ELevel::STATIC);
	Panel_damage.isFullScreen = false;
	Panel_damage.isTransparent = false;
	Panel_damage.isUseLayout = true;

	Panel_damage.layoutDesc.spacing = { 2.f ,2.f };
	Panel_damage.layoutDesc.col = 0;
	Panel_damage.layoutDesc.row = 3;
	//Panel_damage.LayoutDesc.m_Offset = { 0.f, 0.f};

	Panel_damage.useNineSlice = true;

	shared_ptr<CUIPanel> DamgePanel = CUIPanel::Create(m_device, m_context);
	DamgePanel->Initialize(&Panel_damage);

	for (_uint i = 0; i < 3; i++)
	{

		CUISlot::SlotDesc slot_Damage = {};
		slot_Damage.textureProtoName = L"Prototype_Component_Texture_Slot_Damage";
		slot_Damage.textureComLevel = ETOI(ELevel::STATIC);
		slot_Damage.useDark = false;
		/*	slot_Damage.Interaction = false;*/
		shared_ptr<CUISlot> m_pInstance = CUISlot::Create(m_device, m_context);
		m_pInstance->Initialize(&slot_Damage);

		wstring NameTag = L"SLOT_" + S2W(to_string(i));


		DamgePanel->Add_Layout_Child(m_pInstance, NameTag, false);
		m_pInstance->Set_Interactive(false);
		m_pInstance->Set_Zorder(2);
		m_damageSlot.push_back(m_pInstance);
	}


	Add_Child(DamgePanel, L"Panel_damage", false);


	{

	}



	hr = __super::OnInit(pDesc);
	return hr;
}

void CUI_Inventory::OnActive()
{
	__super::OnActive();

}

void CUI_Inventory::OnInActive()
{

	Evt_MouseToIndexData e = {};
	e.IsOnSlot = false; // 기본 false

	//if (m_Inven.lock()->Get_Inventype() == INVENTYPE::PLAYER)
	//{
		e.IsPlayer = true;
	//}
	//else
	//{
	//	e.IsPlayer = false;
	//}

	


	

	e.x = 0;
		e.y = 0;
			e.IsOnSlot = false;



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
	auto& InvenItem = m_inven.lock()->Get_InventoryItem();
	auto& InvenSlot = m_inven.lock()->Get_Invenslot();

	for (_uint index = 0; index < InvenSlot.size(); index++)
	{
		_float4 Muscolor={};
		_float4 colorReset = { 0.f,0.f,0.f,0.f };

		m_slot[index]->Set_ColorFlat(colorReset);
		switch (InvenSlot[index].slotColor)
		{
		case EPlaceColor::GREEN:
			Muscolor = { 0.f,1.f,0.f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_slot[index]->Set_ColorFlat(Muscolor);
		
			break;
		case EPlaceColor::ORANGE:
			Muscolor = { 1.0f,0.6f,0.0f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_slot[index]->Set_ColorFlat(Muscolor);

			break;
		case EPlaceColor::RED:
			Muscolor = { 1.f,0.f,0.f,.8f };
			//m_Slot[index]->Set_UseColorMix(true);
			m_slot[index]->Set_ColorFlat(Muscolor);
			break;
		case EPlaceColor::END:
		
			//m_Slot[index]->Set_ColorFlat(false);
			break;
		}


		if(InvenSlot[index].isLock == false)
		{
			if (InvenSlot[index].isBroken == true)
			{
				m_slot[index]->Set_TextureIndex(1);
			}
			else
			{
				m_slot[index]->Set_TextureIndex(0);
			}
		}
		
	}


	for (auto& invnelItem : InvenItem)
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
	e.IsOnSlot = false; // 기본 false

	if(m_inven.lock()->Get_Inventype() == EInvenType::PLAYER)
	{
		e.IsPlayer = true;
	}
	else
	{
		e.IsPlayer = false;
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
			e.IsOnSlot = true;

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

	for (auto& UI : m_itemUi)
	{
		UI->Set_Transparent(true);
	}
	for (int i = 0; i < items.size(); i++)
	{
		

		LayoutDesc layout = m_invenPanel->Get_LayoutDesc();

		for (auto& UI : m_itemUi)
		{
			if (UI->Get_Transparent() == true)
			{
				ItemDef def = CItemDB::GetInstance()->GetItemByID(items[i].itemDefId);
				UI->Change_Texture(items[i].cachingTexture);

				_float2 vAnchoredPos = Calculate_RenderPos(items[i]);

				UI->GetUITransform()->SetAnchoredPos(vAnchoredPos);
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
	m_itemUi.clear();
	m_slot.clear();

}
