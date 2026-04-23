#include "Inventory_Controller.h"

#include "Inventory.h"
#include "GameInstance.h"
#include "EventBus.h"

#include "Client_Enum.h"
#include "DInput_Manager.h"
#include "UI_Item.h"
#include "ItemDB.h"
#include "PlayerBoat.h"
#include "UI_Inventory.h"


//IMPLEMENT_SINGLETON(CInventory_Controller)

CInventory_Controller::CInventory_Controller(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CInventory_Controller::Initialize(weak_ptr<CInventory> Inven, shared_ptr<CUI_Item> UIHoldItem)
{

	m_PlayerInven = Inven;
	m_UIHoldItem = UIHoldItem;
	////m_Inven = CInventory::Create();
	//
	////m_Inven->Upgrade_Boat(0); // 젤 처음
	///
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_OpenInventory>(
		[this](const Evt_OpenInventory& e)
		{
			m_TargetInven = e.inven;
		});
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_CloseInventory>(
		[this](const Evt_CloseInventory&)
		{
			m_TargetInven.reset();
		});
	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_MouseToIndex_Data>([this](const Evt_MouseToIndex_Data& e) {m_SlotX = e.x; m_SlotY = e.y; m_bIsOnSlot = e.IsOnSlot;  m_IsPlayerSlot = e.IsPlayer;  });

	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_GetFish>([this](const Evt_GetFish& e)
	{
			Make_Hold(m_PlayerInven.lock()->Create_ItemInstance(e.DefID, e.fishInst));

	});

	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_Demage>([this](const Evt_Demage& e)
		{
			m_PlayerInven.lock()->Get_Damage();

		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_MoveTo_Storage>([this](const Evt_MoveTo_Storage e)
		{
			auto player = dynamic_pointer_cast<CPlayerBoat>(m_PlayerInven.lock()->Get_GOwner());
			if (player == nullptr)
				return;
			m_PlayerInven.lock()->Auto_Move_To(player->Get_StorageCom(), m_SlotX, m_SlotY);
		});
	//auto tmppointer = dynamic_pointer_cast<CInventory_Controller>(shared_from_this());
	//if(tmppointer == nullptr)
	//{
	//	return E_FAIL;
	//}
	//EvtControllerPoiner p = {};
	//p.m_contrl_Pointer = tmppointer;
	//m_pGameInstance.lock()->Get_EventBus()->Publish(p);

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_FixAll>([this](const Evt_FixAll& e)
		{
			auto player = dynamic_pointer_cast<CPlayerBoat>(m_PlayerInven.lock()->Get_GOwner());
			if (player->GetMoney() >= player->Get_DemageFixPrice())
			{
				player->MinusMoney(player->Get_DemageFixPrice());
				m_PlayerInven.lock()->FixAll();

				Evt_FixAll_Done event{};

				CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

			}
		});


	return S_OK;
}
void CInventory_Controller::Make_Hold(Item_Inst inst)
{
	
		m_UIHoldItem->HoldItem(inst);

		m_bDragging = true;
	
}
shared_ptr<CInventory>  CInventory_Controller::Get_CurrentInven()
{
	if (m_IsPlayerSlot)
		return m_PlayerInven.lock();
	else
		return m_TargetInven.lock();
}
void CInventory_Controller::Update(float TimeDelta)
{
	auto dInput = m_pGameInstance.lock()->Get_DInput_Manger();
	//	->MouseDown(DIMB::LBUTTON);


	auto CurInven = Get_CurrentInven();
	if (CurInven == nullptr)
	{
		return;
	}

	/*_uint SlotX, SlotY = { 0 };*/



	if (m_bIsOnSlot == true/*m_UIInven->MousePosToSlot(SlotX, SlotY)*/)/*ui 에서 마우스가 어느 칸을 가르키고 있는지 반환 , -면 칸 이 아닌거임*/
	{
		m_bPrevOnSlot = true;
		// 마우스가 그리드 안에 들어와 있다
		Item_Inst tmpInst = {};


		if (/*m_HoldItem.ItemInst_ID == ID_Absence &&*/ m_bDragging == false /* + ui 가 클릭은 반환*/)
		{
		m_PrevDragging = false; 
			if (dInput->MouseDown(DIMB::LBUTTON) /* + ui 가 클릭은 반환*/)
			{
				//잡고 있는 아이템이 없을경우
				//
				if(CurInven->Get_Inventype() == INVENTYPE::SHOP)
				{
					Item_Inst inst = CurInven->Peek_Itme(m_SlotX, m_SlotY);
					const Item_Def def = CItemDB::GetInstance()->GetItemByID(inst.ItemDef_ID);
					
						if (auto* equip = get_if<Equip_Def>(&def.TypeDef))
						{
							auto player = dynamic_pointer_cast<CPlayerBoat>(m_PlayerInven.lock()->Get_GOwner());
							if(player->GetMoney() >= equip->Cost)
							{
								player->MinusMoney(equip->Cost);
								// 집기
								tmpInst = CurInven->TryMove_Item(m_SlotX, m_SlotY);

								if (tmpInst.ItemInst_ID == ID_Absence)
									return;
								//MSG_BOX("Faild : TryMove_Item");

							//m_HoldItem = tmpInst;
								m_UIHoldItem->HoldItem(tmpInst);

								m_bDragging = true;
							}
						}
				}
				else
				{
					// 집기
					tmpInst = CurInven->TryMove_Item(m_SlotX, m_SlotY);

					if (tmpInst.ItemInst_ID == ID_Absence)
						return;
					//MSG_BOX("Faild : TryMove_Item");

				//m_HoldItem = tmpInst;
					m_UIHoldItem->HoldItem(tmpInst);

					m_bDragging = true;
				}

			}
			else if (dInput->KeyDown(DIK_Z) /* + 일정 시간 이상 누르고 있을떄*/)
			{
				//인밴에 있는거 버리기
				CurInven->ThrowAwayFrom_Inven(m_SlotX, m_SlotY);
			}
			//auto Inven = m_PlayerInven.lock();
			Item_Inst inst = CurInven->Peek_Itme(m_SlotX, m_SlotY);
			if(m_PrevSlotX != m_SlotX || m_PrevSlotY != m_SlotY|| inst.ItemInst_ID!= m_prevItemInstId)
			{
				Evt_ItemHovered e = {};
				e.isHold = false;
				e.itemInst = inst;
				if (Get_CurrentInven() == m_PlayerInven.lock())
				{
					e.IsPlayer = true;
				}
				else
				{
					e.IsPlayer = false;
				}
				m_pGameInstance.lock()->Get_EventBus()->Publish(e);

				m_PrevSlotX = m_SlotX;
				m_PrevSlotY = m_SlotY;
				m_prevItemInstId = inst.ItemInst_ID;
				
			}
			
		}
		else if (m_bDragging == true && m_bIsOnSlot == true)
		{

			// 잡고있는 아이템이 있는경우
			PLACE_COLOR color = PLACE_COLOR::END;

			CurInven->CanPlace(/*m_HoldItem*/m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY, color);

			CurInven->SetHighlightArea(m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY, color);

			
		

			if (dInput->MouseDown(DIMB::LBUTTON))
			{
				auto DestInven = Get_CurrentInven();
				switch (color)
				{
				case PLACE_COLOR::GREEN:
					tmpInst = DestInven->AddItem(m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY);
					//m_HoldItem = tmpInst;// 이건 빈 인스턴스
					m_UIHoldItem->ReleaseItem();
					m_bDragging = false;
					break;
				case PLACE_COLOR::ORANGE:
					tmpInst = DestInven->AddItem(m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY);

					if (tmpInst.ItemInst_ID == ID_Absence)
						return;
					//MSG_BOX("Faild : Get Swap Item from AddItem");

				//m_HoldItem = tmpInst;
					m_UIHoldItem->HoldItem(tmpInst);
					m_bDragging = true;
					break;
				case PLACE_COLOR::RED:
					// 레드 일때 애니메이션 뭐 그런거 할거 있음 여기
					// 홀드 아이템은 그대로
					m_bDragging = true;

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
			if(m_bPrevOnSlot == true && m_bDragging ==false)
			{
			Item_Inst emptyinst = {};

			Evt_ItemHovered e = {};
			e.isHold = false;
			e.itemInst = emptyinst;
			if(Get_CurrentInven() == m_PlayerInven.lock())
			{
				e.IsPlayer = true;
			}
			else
			{
				e.IsPlayer = false;
			}

			
			m_pGameInstance.lock()->Get_EventBus()->Publish(e);

			m_PrevSlotX = 99;
			m_PrevSlotY = 99;
				
			}
			m_bPrevOnSlot = false;
			//prevMouseOnSlot = false;

		//}
	}

	if (m_bDragging == true)
	{
		
		if (dInput->MouseDown(DIMB::RBUTTON)/*dInput->MouseDown(DIMB::RBUTTON)*/ /* + ui 가 우클릭은 반환*/)
		{
			//회전
			_uint rot = m_UIHoldItem->Get_HoldItem().Rotation;
			m_UIHoldItem->Set_Rotation(++rot);
		}
		if (dInput->KeyDown(DIK_Z)/* + 일정 시간 이상 누르고 있을떄 */)
		{
			//들고 있는거 버리기
			//Item_Inst NoInst = {};
			//m_HoldItem = NoInst;

			m_bDragging = false;
			m_UIHoldItem->ReleaseItem();
			//색칠한거 지워야함
			//auto inven = Get_CurrentInven();
			for(auto& slot : CurInven->Get_Invenslot())
			{
				slot.Slot_Color = PLACE_COLOR::END;
			}
		}
		if (m_bIsOnSlot == false)
		{

			//auto inven = Get_CurrentInven();
			for (auto& slot : CurInven->Get_Invenslot())
			{
				slot.Slot_Color = PLACE_COLOR::END;
			}
		}

		if (m_bDragging != m_PrevDragging/* m_PrevDragging != m_bDragging*/ /*|| m_PrevSlotX != m_SlotX || m_PrevSlotY != m_SlotY*/)
		{
			//if (m_PrevDragging == false) // 처음 들었을 때만
			//{
			Evt_ItemHovered e = {};
			e.isHold = true;
			e.itemInst = m_UIHoldItem->Get_HoldItem();

			m_pGameInstance.lock()->Get_EventBus()->Publish(e);

			m_PrevDragging = m_bDragging;
			//m_PrevSlotX = m_SlotX;
			//m_PrevSlotY = m_SlotY;
		}

	}
	auto cur = CurInven;
	auto player = m_PlayerInven.lock();

	if (cur && player && cur.get() == player.get() && player->Get_Dirty())
	{
	
		player->Set_Dirty(false);
		Evt_ShipStat stat = player->CalculateEquip();
		dynamic_pointer_cast<CPlayerBoat>(player->Get_GOwner())->Set_ShipStats(stat.EngineSpeed, stat.FishingSpeed, stat.LightIntensity, stat.SeaMask, stat.InvenMoney);
	
	}


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

