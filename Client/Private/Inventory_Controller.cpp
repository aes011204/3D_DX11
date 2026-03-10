#include "Inventory_Controller.h"

#include "Inventory.h"
#include "InventoryUI.h"
#include "GameInstance.h"
#include "EventBus.h"
#include "Event_Struct.h"
#include "Client_Enum.h"

CInventory_Controller::CInventory_Controller() :
 m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CInventory_Controller::Initialize()
{
	m_Inven = CInventory::Create();

	m_Inven->Upgrade_Boat(0); // 젤 처음

	Evt_UIslot_Data e = {};
	e.h = m_Inven->Get_H();
	e.w = m_Inven->Get_W();
	e.InvenSlot = m_Inven->Get_Invenslot();

	m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_UIslot_Data>(e);


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

void CInventory_Controller::Update()
{


	_uint SlotX, SlotY = { 0 };

	if (m_UIInven->MousePosToSlot(SlotX, SlotY)/*ui 에서 마우스가 어느 칸을 가르키고 있는지 반환 , -면 칸 이 아닌거임*/)
	{
		// 마우스가 그리드 안에 들어와 있다
		Item_Inst tmpInst = {};

		if (/*m_HoldItem.ItemInst_ID == ID_Absence && */Is_Dragging == false /* + ui 가 클릭은 반환*/)
		{
			if (true /* + ui 가 클릭은 반환*/)
			{
				//잡고 있는 아이템이 없을경우
				// 집기
				tmpInst = m_Inven->TryMove_Item(SlotX, SlotY);

				if (tmpInst.ItemInst_ID == ID_Absence)
					MSG_BOX("Faild : TryMove_Item");

				m_HoldItem = tmpInst;

				Is_Dragging = true;
			}
			else if (true /* + ui 가 z 반환+일정 시간 이상 누르고 있을떄*/)
			{
				//인밴에 있는거 버리기
				m_Inven->ThrowAwayFrom_Inven(SlotX, SlotY);
			}
		}
		else if (Is_Dragging == true)
		{
			// 잡고있는 아이템이 있는경우
			PLACE_COLOR color = PLACE_COLOR::END;

			m_Inven->CanPlace(m_HoldItem, SlotX, SlotY, color);

			if (true /* + ui 가 클릭은 반환*/)
			{

				switch (color)
				{
				case PLACE_COLOR::GREEN:
					tmpInst = m_Inven->AddItem(m_HoldItem, SlotX, SlotY);
					m_HoldItem = tmpInst;// 이건 빈 인스턴스
					Is_Dragging = false;
					break;
				case PLACE_COLOR::ORANGE:
					tmpInst = m_Inven->AddItem(m_HoldItem, SlotX, SlotY);

					if (tmpInst.ItemInst_ID == ID_Absence)
						MSG_BOX("Faild : Get Swap Item from AddItem");

					m_HoldItem = tmpInst;
					Is_Dragging = true;
					break;
				case PLACE_COLOR::RED:
					// 레드 일때 애니메이션 뭐 그런거 할거 있음 여기
					break;
				}
			}
			else if(true /* + ui 가 우클릭은 반환*/) 
			{
				//회전
				m_HoldItem.Rotation++;
			}
			else if (true /* + ui 가 z 반환 +일정 시간 이상 누르고 있을떄 */)
			{
				//들고 있는거 버리기
				Item_Inst NoInst = {};
				m_HoldItem = NoInst;

				Is_Dragging = false;

			}
			//색반환 드레그 중이면
			{
				// ui 에 색 반환
				//ui-color전달
			}

		}


	}


}

shared_ptr<CInventory_Controller> CInventory_Controller::Create()
{
	shared_ptr<CInventory_Controller> pInstance(new CInventory_Controller(),
		[](CInventory_Controller* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CInventory_Controller");
	}
	return pInstance;
}

void CInventory_Controller::Free()
{
	__super::Free();

}
