#include "Inventory_Controller.h"

#include "Inventory.h"
#include "GameInstance.h"
#include "EventBus.h"
#include "Event_Struct.h"
#include "Client_Enum.h"
#include "DInput_Manager.h"
#include "UI_Item.h"
#include "ItemDB.h"
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


	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_MouseToIndex_Data>([this](const Evt_MouseToIndex_Data& e) {m_SlotX = e.x; m_SlotY = e.y; m_bIsOnSlot = e.IsOnSlot; });


	//auto tmppointer = dynamic_pointer_cast<CInventory_Controller>(shared_from_this());
	//if(tmppointer == nullptr)
	//{
	//	return E_FAIL;
	//}
	//EvtControllerPoiner p = {};
	//p.m_contrl_Pointer = tmppointer;
	//m_pGameInstance.lock()->Get_EventBus()->Publish(p);

	return S_OK;
}

void CInventory_Controller::Update(float TimeDelta)
{
	auto dInput = m_pGameInstance.lock()->Get_DInput_Manger();
	//	->MouseDown(DIMB::LBUTTON);


	auto Inven = m_PlayerInven.lock();
	if (Inven == nullptr)
	{
		return;
	}

	/*_uint SlotX, SlotY = { 0 };*/



	if (m_bIsOnSlot == true/*m_UIInven->MousePosToSlot(SlotX, SlotY)*/)/*ui 에서 마우스가 어느 칸을 가르키고 있는지 반환 , -면 칸 이 아닌거임*/
	{
		// 마우스가 그리드 안에 들어와 있다
		Item_Inst tmpInst = {};


		if (/*m_HoldItem.ItemInst_ID == ID_Absence &&*/ Is_Dragging == false /* + ui 가 클릭은 반환*/)
		{
		m_PrevDragging = false; 
			if (dInput->MouseDown(DIMB::LBUTTON) /* + ui 가 클릭은 반환*/)
			{
				//잡고 있는 아이템이 없을경우
				// 집기
				tmpInst = Inven->TryMove_Item(m_SlotX, m_SlotY);

				if (tmpInst.ItemInst_ID == ID_Absence)
					return;
				//MSG_BOX("Faild : TryMove_Item");

			//m_HoldItem = tmpInst;
				m_UIHoldItem->HoldItem(tmpInst);

				Is_Dragging = true;
			}
			else if (dInput->KeyDown(DIK_Z) /* + 일정 시간 이상 누르고 있을떄*/)
			{
				//인밴에 있는거 버리기
				Inven->ThrowAwayFrom_Inven(m_SlotX, m_SlotY);
			}
			//auto Inven = m_PlayerInven.lock();
			Item_Inst inst = m_PlayerInven.lock()->Peek_Itme(m_SlotX, m_SlotY);
			if(m_PrevSlotX != m_SlotX || m_PrevSlotY != m_SlotY|| inst.ItemInst_ID!= m_prevItemInstId)
			{
				Evt_ToolTip e = {};
				e.isHold = false;
				e.itemInst = inst;
				e.locationState = LOCATIONSTATE::SEA;
				m_pGameInstance.lock()->Get_EventBus()->Publish(e);

				m_PrevSlotX = m_SlotX;
				m_PrevSlotY = m_SlotY;
				m_prevItemInstId = inst.ItemInst_ID;
				
			}
			
		}
		else if (Is_Dragging == true && m_bIsOnSlot == true)
		{

			// 잡고있는 아이템이 있는경우
			PLACE_COLOR color = PLACE_COLOR::END;

			Inven->CanPlace(/*m_HoldItem*/m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY, color);

			Inven->SetHighlightArea(m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY, color);

			
		

			if (dInput->MouseDown(DIMB::LBUTTON))
			{
				switch (color)
				{
				case PLACE_COLOR::GREEN:
					tmpInst = Inven->AddItem(m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY);
					//m_HoldItem = tmpInst;// 이건 빈 인스턴스
					m_UIHoldItem->ReleaseItem();
					Is_Dragging = false;
					break;
				case PLACE_COLOR::ORANGE:
					tmpInst = Inven->AddItem(m_UIHoldItem->Get_HoldItem(), m_SlotX, m_SlotY);

					if (tmpInst.ItemInst_ID == ID_Absence)
						return;
					//MSG_BOX("Faild : Get Swap Item from AddItem");

				//m_HoldItem = tmpInst;
					m_UIHoldItem->HoldItem(tmpInst);
					Is_Dragging = true;
					break;
				case PLACE_COLOR::RED:
					// 레드 일때 애니메이션 뭐 그런거 할거 있음 여기
					// 홀드 아이템은 그대로
					Is_Dragging = true;

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
			if(Is_Dragging ==false)
			{
			Item_Inst emptyinst = {};

			Evt_ToolTip e = {};
			e.isHold = false;
			e.itemInst = emptyinst;
			e.locationState = LOCATIONSTATE::END;
			m_pGameInstance.lock()->Get_EventBus()->Publish(e);

			m_PrevSlotX = 99;
			m_PrevSlotY = 99;
				
			}
			//prevMouseOnSlot = false;

		//}
	}

	if (Is_Dragging == true)
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

			Is_Dragging = false;
			m_UIHoldItem->ReleaseItem();
			//색칠한거 지워야함
			for(auto& slot :m_PlayerInven.lock()->Get_Invenslot())
			{
				slot.Slot_Color = PLACE_COLOR::END;
			}
		}
		if (m_bIsOnSlot == false)
		{


			for (auto& slot : m_PlayerInven.lock()->Get_Invenslot())
			{
				slot.Slot_Color = PLACE_COLOR::END;
			}
		}

		if (Is_Dragging != m_PrevDragging/* m_PrevDragging != Is_Dragging*/ /*|| m_PrevSlotX != m_SlotX || m_PrevSlotY != m_SlotY*/)
		{
			//if (m_PrevDragging == false) // 처음 들었을 때만
			//{
			Evt_ToolTip e = {};
			e.isHold = true;
			e.itemInst = m_UIHoldItem->Get_HoldItem();
			e.locationState = LOCATIONSTATE::SEA;
			m_pGameInstance.lock()->Get_EventBus()->Publish(e);

			m_PrevDragging = Is_Dragging;
			//m_PrevSlotX = m_SlotX;
			//m_PrevSlotY = m_SlotY;
		}

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

