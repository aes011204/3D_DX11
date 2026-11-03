#include "Inventory_Controller.h"

#include <numbers>

#include "Inventory.h"
#include "GameInstance.h"
#include "EventBus.h"

#include "Client_Enum.h"
#include "DInput_Manager.h"
#include "UI_Item.h"
#include "ItemDB.h"
#include "PlayerBoat.h"
#include "UI_Controller.h"
#include "UI_Inventory.h"


//IMPLEMENT_SINGLETON(CInventory_Controller)

CInventory_Controller::CInventory_Controller(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext),
	m_gameInstance(CGameInstance::GetInstance())
{
}

HRESULT CInventory_Controller::Initialize(weak_ptr<CInventory> Inven, shared_ptr<CUI_Item> UIHoldItem)
{

	m_playerInven = Inven;
	m_uiHoldItem = UIHoldItem;
	////m_Inven = CInventory::Create();
	//
	////m_Inven->Upgrade_Boat(0); // 젤 처음
	///
    m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_ItemSold>(
        [this](const Evt_ItemSold& event)
        {
            if (m_uiHoldItem &&
                m_uiHoldItem->Get_HoldItem().itemInstId == event.itemInstId)
            {
                m_uiHoldItem->ReleaseItem();
                m_isDragging = false;
            }
            for (auto inventory : { m_playerInven.lock(), m_targetInven.lock() })
            {
                if (!inventory)
                    continue;
                const auto& items = inventory->Get_InventoryItem();
                const auto found = find_if(items.begin(), items.end(),
                    [&](const ItemInst& item) { return item.itemInstId == event.itemInstId; });
                if (found != items.end())
                    inventory->RemoveFrom_Inven(event.itemInstId);
                inventory->ClearHighlightArea();
            }
            m_hoverDirty = true;
            CUI_Controller::GetInstance()->CloseToolTip();
        });

	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_OpenInventory>(
		[this](const Evt_OpenInventory& e)
		{
			m_targetInven = e.inven;

		});
	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_CloseInventory>(
		[this](const Evt_CloseInventory&)
		{

			CUI_Controller::GetInstance()->CloseToolTip();
			CUI_Controller::GetInstance()->InActiveHover();
			m_targetInven.reset();

		});
	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_MouseToIndexData>([this](const Evt_MouseToIndexData& e) {m_slotX = e.x; m_slotY = e.y; m_isOnSlot = e.IsOnSlot;  m_isPlayerSlot = e.IsPlayer;  });

	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_GetFish>([this](const Evt_GetFish& e)
		{
			Make_Hold(m_playerInven.lock()->Create_ItemInstance(e.DefID, e.fishInst));

		});

	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_Damage>([this](const Evt_Damage& e)
		{
			m_playerInven.lock()->Get_Damage();

		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_MoveToStorage>([this](const Evt_MoveToStorage e)
		{
			auto playerInven = m_playerInven.lock();
			if (playerInven == nullptr)
				return;

			auto player = dynamic_pointer_cast<CPlayerBoat>(playerInven->Get_GOwner());
			if (player == nullptr)
				return;

			auto storage = player->Get_StorageCom().lock();
			if (storage == nullptr)
				return;

			//  플레이어 인벤에서 집은 홀드 아이템이 있으면 그걸 먼저 창고로 보냄
			if (m_isDragging == true &&
				m_holdItemFrom == EInvenType::PLAYER &&
				m_uiHoldItem != nullptr)
			{
				ItemInst& holdItem = m_uiHoldItem->Get_HoldItem();

				if (holdItem.itemInstId != kIdAbsence)
				{
					if (storage->Auto_Add(holdItem))
					{
						if (holdItem.itemType == EItemType::FISH)
							m_gameInstance.lock()->Play_Once(L"OrganicItem_PickUp");
						else
							m_gameInstance.lock()->Play_Once(L"InorganicItem_PickUp");

						m_uiHoldItem->ReleaseItem();
						m_isDragging = false;
						playerInven->ClearHighlightArea();
					}

					return;
				}
			}

			ItemInst hoveredItem = playerInven->Peek_Item(m_slotX, m_slotY);
			if (hoveredItem.itemInstId == kIdAbsence)
				return;

			if (hoveredItem.itemType == EItemType::FISH)
				m_gameInstance.lock()->Play_Once(L"OrganicItem_PickUp");
			else
				m_gameInstance.lock()->Play_Once(L"InorganicItem_PickUp");

			playerInven->Auto_Move_To(player->Get_StorageCom(), m_slotX, m_slotY);
			playerInven->ClearHighlightArea();
		});
	

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_FixAll>([this](const Evt_FixAll& e)
		{
			auto player = dynamic_pointer_cast<CPlayerBoat>(m_playerInven.lock()->Get_GOwner());
			if (player->GetMoney() >= player->Get_DamageFixPrice())
			{
				player->MinusMoney(player->Get_DamageFixPrice());
				m_playerInven.lock()->FixAll();

				Evt_FixAllDone event{};

				CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

				m_gameInstance.lock()->Play_Once(L"Repair_Kit");


			}
		});


	return S_OK;
}
void CInventory_Controller::Make_Hold(ItemInst inst)
{

	m_uiHoldItem->HoldItem(inst);
	m_holdItemFrom = EInvenType::NONE;
	m_isDragging = true;

	///

}
shared_ptr<CInventory>  CInventory_Controller::Get_CurrentInven()
{
	if (m_isPlayerSlot)
		return m_playerInven.lock();
	else
		return m_targetInven.lock();
}
void CInventory_Controller::Update(float TimeDelta)
{
    auto refreshFinalHover = [this]()
    {
        Refresh_Hover();
        m_isOnSlot = false;
    };
    struct FinalHoverRefresh
    {
        decltype(refreshFinalHover)& refresh;
        ~FinalHoverRefresh() { refresh(); }
    } finalHoverRefresh{ refreshFinalHover };
	auto dInput = m_gameInstance.lock()->Get_DInput_Manger();
	//	->MouseDown(DIMB::LBUTTON);


	auto CurInven = Get_CurrentInven();
	if (CurInven == nullptr)
	{
		return;
	}

	/*_uint SlotX, SlotY = { 0 };*/



	if (m_isOnSlot == true/*m_UIInven->MousePosToSlot(SlotX, SlotY)*/)/*ui 에서 마우스가 어느 칸을 가르키고 있는지 반환 , -면 칸 이 아닌거임*/
	{
		m_isPrevOnSlot = true;
		// 마우스가 그리드 안에 들어와 있다
		ItemInst tmpInst = {};


		if (/*m_HoldItem.ItemInst_ID == ID_Absence &&*/ m_isDragging == false /* + ui 가 클릭은 반환*/)
		{
			m_isPrevDragging = false;
			if (dInput->MouseDown(EMouseButton::LBUTTON) /* + ui 가 클릭은 반환*/)
			{
				//잡고 있는 아이템이 없을경우
				//
				if (CurInven->Get_Inventype() == EInvenType::SHOP)
				{
					ItemInst inst = CurInven->Peek_Item(m_slotX, m_slotY);
					if (inst.itemInstId != 0)
					{

						const ItemDef def = CItemDB::GetInstance()->GetItemByID(inst.itemDefId);

						if (auto* equip = get_if<EquipDef>(&def.typeDef))
						{
							auto player = dynamic_pointer_cast<CPlayerBoat>(m_playerInven.lock()->Get_GOwner());
							if (player->GetMoney() >= equip->cost)
							{
								player->MinusMoney(equip->cost);
								// 집기
								tmpInst = CurInven->TryMove_Item(m_slotX, m_slotY);

								if (tmpInst.itemInstId == kIdAbsence)
									return;
								//MSG_BOX("Faild : TryMove_Item");

							//m_HoldItem = tmpInst;
								m_uiHoldItem->HoldItem(tmpInst);
								m_holdItemFrom = CurInven->Get_Inventype();
								m_isDragging = true;
							}
						}
					}
				}
				else
				{
					// 집기
					tmpInst = CurInven->TryMove_Item(m_slotX, m_slotY);

					if (tmpInst.itemInstId == kIdAbsence)
						return;
					//MSG_BOX("Faild : TryMove_Item");

				//m_HoldItem = tmpInst;
					m_uiHoldItem->HoldItem(tmpInst);
					m_holdItemFrom = CurInven->Get_Inventype();
					m_isDragging = true;
				}

			}
			else if (dInput->KeyDown(DIK_Z) /* + 일정 시간 이상 누르고 있을떄*/)
			{
				//인밴에 있는거 버리기
				CurInven->ThrowAwayFrom_Inven(m_slotX, m_slotY);
			}
			//auto Inven = m_PlayerInven.lock();
			ItemInst inst = CurInven->Peek_Item(m_slotX, m_slotY);
			if (m_prevSlotX != m_slotX || m_prevSlotY != m_slotY || inst.itemInstId != m_prevItemInstId)
			{
				Evt_ItemHovered e = {};
				e.isHold = false;
				e.itemInst = inst;
				if (Get_CurrentInven() == m_playerInven.lock())
				{
					e.IsPlayer = true;
				}
				else
				{
					e.IsPlayer = false;
				}
				m_gameInstance.lock()->Get_EventBus()->Publish(e);
                m_hoverDirty = true;

				m_prevSlotX = m_slotX;
				m_prevSlotY = m_slotY;
				m_prevItemInstId = inst.itemInstId;

			}

		}
		else if (m_isDragging == true && m_isOnSlot == true)
		{

			// 잡고있는 아이템이 있는경우
			EPlaceColor color = EPlaceColor::END;

			CurInven->CanPlace(/*m_HoldItem*/m_uiHoldItem->Get_HoldItem(), m_slotX, m_slotY, color);

			CurInven->SetHighlightArea(m_uiHoldItem->Get_HoldItem(), m_slotX, m_slotY, color);




			if (dInput->MouseDown(EMouseButton::LBUTTON))
			{
				auto DestInven = Get_CurrentInven();
				switch (color)
				{
				case EPlaceColor::GREEN:
					tmpInst = DestInven->AddItem(m_uiHoldItem->Get_HoldItem(), m_slotX, m_slotY);
					//m_HoldItem = tmpInst;// 이건 빈 인스턴스

					if (DestInven->Get_Inventype() == EInvenType::PLAYER && m_holdItemFrom == EInvenType::SHOP)
					{
						if (m_uiHoldItem->Get_HoldItem().itemType == EItemType::EQUIP)
						{

							auto curitem = CItemDB::GetInstance()->GetItemByID(m_uiHoldItem->Get_HoldItem().itemDefId);
							if (auto pEquipDef = get_if<EquipDef>(&curitem.typeDef))
							{
								CUI_Controller::GetInstance()->ActiveTime(pEquipDef->installTime, false);
							}
						}
					}
					m_uiHoldItem->ReleaseItem();
					m_isDragging = false;

					CurInven->ClearHighlightArea();





					break;
				case EPlaceColor::ORANGE:
				{
				//	tmpInst = DestInven->AddItem(m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY);
		
				//	if (tmpInst.ItemInst_ID == ID_Absence)
				//		return;
				//	//MSG_BOX("Faild : Get Swap Item from AddItem");
				//	if (DestInven->Get_Inventype() == INVENTYPE::PLAYER && m_HoldItemFrom == INVENTYPE::SHOP)
				//	{
				//		if (m_UIHoldItem->Get_HoldItem().ItemType == ITEM_TYPE::EQUIP)
				//		{

				//			auto curitem = CItemDB::GetInstance()->GetItemByID(m_UIHoldItem->Get_HoldItem().ItemDef_ID);
				//			if (auto pEquipDef = get_if<Equip_Def>(&curitem.TypeDef))
				//			{
				//				CUI_Controller::GetInstance()->ActiveTime(pEquipDef->InstallTime, false);
				//			}
				//		}
				//	}
				////m_HoldItem = tmpInst;
				//	m_UIHoldItem->HoldItem(tmpInst);
				//	m_HoldItemFrom = DestInven->Get_Inventype();
				//	m_bDragging = true;



				//	Evt_ItemHovered evt = {};
				//	evt.isHold = true;
				//	evt.itemInst = m_UIHoldItem->Get_HoldItem();
				//	m_pGameInstance.lock()->Get_EventBus()->Publish(evt);

					
					ItemInst placedItem = m_uiHoldItem->Get_HoldItem();

					tmpInst = DestInven->AddItem(placedItem, m_slotX, m_slotY);

					if (tmpInst.itemInstId == kIdAbsence)
						return;

					// 먼저 "방금 놓은 아이템" 기준으로 ActiveTime 판정
					if (DestInven->Get_Inventype() == EInvenType::PLAYER && m_holdItemFrom == EInvenType::SHOP)
					{
						if (placedItem.itemType == EItemType::EQUIP)
						{
							auto curitem = CItemDB::GetInstance()->GetItemByID(placedItem.itemDefId);
							if (auto pEquipDef = get_if<EquipDef>(&curitem.typeDef))
							{
								CUI_Controller::GetInstance()->ActiveTime(pEquipDef->installTime, false);
							}
						}
					}

					// 그 다음 홀드 아이템을 교체 후 아이템으로 갱신
					m_uiHoldItem->HoldItem(tmpInst);
					m_holdItemFrom = DestInven->Get_Inventype();
					m_isDragging = true;

					// 새 홀드 아이템 툴팁 갱신
					Evt_ItemHovered e = {};
					e.isHold = true;
					e.itemInst = m_uiHoldItem->Get_HoldItem();
					CUI_Controller::GetInstance()->CacheToolTip(e);

					break;
					break;
				}
				case EPlaceColor::RED:
					// 레드 일때 애니메이션 뭐 그런거 할거 있음 여기
					// 홀드 아이템은 그대로
					m_isDragging = true;

					break;
				}




			}

			//색반환 드레그 중이면
			{
				// ui 에 색 반환
				//ui-color전달
			}

		}


	}
	else
	{
		// 밖으로 나갓을때
		//if (prevMouseOnSlot == true)
		//{
			//if( m_PrevSlotX != m_SlotX || m_PrevSlotY != m_SlotY)
		if (m_isPrevOnSlot == true && m_isDragging == false)
		{
			ItemInst emptyinst = {};

			Evt_ItemHovered e = {};
			e.isHold = false;
			e.itemInst = emptyinst;
			if (Get_CurrentInven() == m_playerInven.lock())
			{
				e.IsPlayer = true;
				//m_PlayerInven.lock()->ClearHighlightArea();
				//m_TargetInven.lock()->ClearHighlightArea();
			}
			else
			{
				e.IsPlayer = false;
				//m_TargetInven.lock()->ClearHighlightArea();
				//m_PlayerInven.lock()->ClearHighlightArea();
			}


			m_gameInstance.lock()->Get_EventBus()->Publish(e);
                m_hoverDirty = true;

			m_prevSlotX = 99;
			m_prevSlotY = 99;

		}
		m_isPrevOnSlot = false;
		//prevMouseOnSlot = false;

	//}
	}

	if (m_isDragging == true)
	{

		if (dInput->MouseDown(EMouseButton::RBUTTON)/*dInput->MouseDown(DIMB::RBUTTON)*/ /* + ui 가 우클릭은 반환*/)
		{
			//회전
			_uint rot = m_uiHoldItem->Get_HoldItem().rotation;
			m_uiHoldItem->Set_Rotation(++rot);
			m_gameInstance.lock()->Play_Once(L"Rotate");
		}
		if (dInput->KeyDown(DIK_Z)/* + 일정 시간 이상 누르고 있을떄 */)
		{
			//들고 있는거 버리기
			//Item_Inst NoInst = {};
			//m_HoldItem = NoInst;

			m_isDragging = false;
			m_uiHoldItem->ReleaseItem();
			//색칠한거 지워야함
			//auto inven = Get_CurrentInven();
			CurInven->ClearHighlightArea();
		}
		if (m_isOnSlot == false)
		{

			//auto inven = Get_CurrentInven();
			if (auto p = m_playerInven.lock()) p->ClearHighlightArea();
			if (auto t = m_targetInven.lock()) t->ClearHighlightArea();
		}

		if (m_isDragging != m_isPrevDragging/* m_PrevDragging != m_bDragging*/ /*|| m_PrevSlotX != m_SlotX || m_PrevSlotY != m_SlotY*/)
		{
			//if (m_PrevDragging == false) // 처음 들었을 때만
			//{
			Evt_ItemHovered e = {};
			e.isHold = true;
			e.itemInst = m_uiHoldItem->Get_HoldItem();

			m_gameInstance.lock()->Get_EventBus()->Publish(e);
                m_hoverDirty = true;

			m_isPrevDragging = m_isDragging;
			//m_PrevSlotX = m_SlotX;
			//m_PrevSlotY = m_SlotY;
		}

	}
	auto cur = CurInven;
	auto player = m_playerInven.lock();

	if (cur && player && cur.get() == player.get() && player->Get_Dirty())
	{

		player->Set_Dirty(false);
		Evt_ShipStat stat = player->CalculateEquip();
		dynamic_pointer_cast<CPlayerBoat>(player->Get_GOwner())->Set_ShipStats(stat.engineSpeed, stat.fishingSpeed, stat.lightIntensity, stat.seaMask, stat.invenMoney);

	}



}

void CInventory_Controller::Refresh_Hover()
{
    auto inventory = Get_CurrentInven();
    auto playerInventory = m_playerInven.lock();
    Evt_ItemHovered event = {};
    event.isHold = m_isDragging;
    event.IsPlayer = inventory && inventory == playerInventory;

    if (m_isDragging && m_uiHoldItem)
        event.itemInst = m_uiHoldItem->Get_HoldItem();
    else if (m_isOnSlot && inventory)
        event.itemInst = inventory->Peek_Item(m_slotX, m_slotY);

    const bool changed = !m_hoverValid || m_hoverDirty ||
        m_hoverInventory.lock() != inventory ||
        m_hoverState.isHold != event.isHold ||
        m_hoverState.IsPlayer != event.IsPlayer ||
        m_hoverState.itemInst.itemInstId != event.itemInst.itemInstId ||
        m_hoverState.itemInst.rotation != event.itemInst.rotation;
    if (!changed)
        return;

    m_hoverState = event;
    m_hoverInventory = inventory;
    m_hoverValid = true;
    m_hoverDirty = false;
    m_gameInstance.lock()->Get_EventBus()->Publish(event);
}

shared_ptr<CInventory_Controller> CInventory_Controller::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, weak_ptr<CInventory> Inven, shared_ptr<CUI_Item> UIHoldItem)
{
	shared_ptr<CInventory_Controller> pInstance(new CInventory_Controller(pDevice, pContext),
		[](CInventory_Controller* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(Inven, UIHoldItem)))
	{
		MSG_BOX("Failed to Created : CInventory_Controller");
	}
	return pInstance;
}

void CInventory_Controller::Free()
{
	__super::Free();

}