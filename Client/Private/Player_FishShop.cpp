#include "Player_FishShop.h"

#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"
#include "Inventory.h"
#include "ItemDB.h"
#include "UI_NPC.h"

CPlayer_FishShop::CPlayer_FishShop(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> stateMachine)
	: CPlayerState(owner, stateMachine)
{
}

CPlayer_FishShop::~CPlayer_FishShop()
{
}

void CPlayer_FishShop::Enter()
{

	CDialogueDB::GetInstance()->Set_PendingDialogue(m_dialogueList);

	Evt_ChangeCam event = {};
	auto lerp = make_shared<CamLerpDesc>();
	lerp->mode = ECamMode::LERP;
	lerp->targetPos = _float3(0.688f, 2.814f, 1.268f);
	lerp->targetRot = _float3(21.49f, -50.13f, 0.f);
	lerp->duration = 1.0f;
	//pLerp->fFov = 30.f;
	lerp->OnComplete = [this]() {
		Evt_CamArrived e = {};
		e.playerstate = EPlayerState::FISH_SHOP;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
		};
	event.commands.push_back(lerp);

	auto stop = make_shared<CamDesc>();
	stop->mode = ECamMode::STOP;
	event.commands.push_back(stop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
	m_nextState = EPlayerState::FISH_SHOP;


	CPlayerState::Enter();
}

void CPlayer_FishShop::Exit()
{
	//auto m_NPC = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC");
	//m_NPC->UI_InActive();
	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

	CPlayerState::Exit();
}

HRESULT CPlayer_FishShop::Init_State()
{
	m_dialogueList.push_back("FishStore_First");

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeState>([this](const Evt_ChangeState& e)
		{
			m_nextState = e.playerstate;
		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ItemHovered>([this](const Evt_ItemHovered& e)
		{
			m_itemIDInst = e.itemInst;
		});

	return CPlayerState::Init_State();
}

int CPlayer_FishShop::Update_State(const _float& timeDelta)
{

	if (m_inputManager->KeyDown(DIK_X))
	{

		return  ETOI(EPlayerState::VILLAGE);
	}
	else if (m_inputManager->KeyDown(DIK_F) && !m_itemIDInst.itemInstID == kIDAbsence)
	{
		if (m_itemIDInst.itemType == EItemType::FISH)
		{
			float money = {};
			if (m_itemIDInst.isMutation == true)
				money = m_itemIDInst.mutationCaching.mutCost;
			else
			{
				auto def = CItemDB::GetInstance()->GetItemByID(m_itemIDInst.itemDefID).typeDef;

				money = get<FishDef>(def).cost;

			}

			m_owner.lock()->Add_Money(money);
			m_owner.lock()->GetInventory()->RemoveFrom_Inven(m_itemIDInst.itemInstID);
            Evt_ItemSold soldEvent = {};
            soldEvent.itemInstID = m_itemIDInst.itemInstID;
            m_itemIDInst = {};
            m_gameInstance.lock()->Get_EventBus()->Publish(soldEvent);
			

		}

	}
	else if (m_inputManager->MouseDown(EMouseButton::WHEEL))
	{
		Evt_MoveToStorage e = {};
		
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

		
	}

	return ETOI(m_nextState);
}

void CPlayer_FishShop::LateUpdate_State(const _float& timeDelta)
{

}

void CPlayer_FishShop::Render_State()
{

}

shared_ptr<CPlayer_FishShop> CPlayer_FishShop::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> stateMachine)
{
	shared_ptr<CPlayer_FishShop> instance(new CPlayer_FishShop(owner, stateMachine), [](CPlayer_FishShop* p) {p->Free(); delete(p); });


	return instance;
}

void CPlayer_FishShop::Free()
{
	CPlayerState::Free();
}
