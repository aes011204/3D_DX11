#include "Player_RepairShop.h"

#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"
#include "ItemDB.h"
#include "Inventory.h"

CPlayer_RepairShop::CPlayer_RepairShop(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> stateMachine)
	: CPlayerState(owner, stateMachine)
{
}

CPlayer_RepairShop::~CPlayer_RepairShop()
{
}

void CPlayer_RepairShop::Enter()
{
	CDialogueDB::GetInstance()->Set_PendingDialogue(m_dialogueList);


	Evt_ChangeCam event = {};
	auto lerp = make_shared<CamLerpDesc>();
	lerp->mode = ECamMode::LERP;
	lerp->targetPos = _float3(3.14f, 3.429f, 5.734f);
	lerp->targetRot = _float3(5.58f, -145.63f, 0.f);
	lerp->duration = 2.0f;
	lerp->FOV = 30.f;
	lerp->OnComplete = [this]() {
		Evt_CamArrived e = {};
		e.playerstate = EPlayerState::REPAIR_SHOP;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
	};
	event.commands.push_back(lerp);

	auto stop = make_shared<CamDesc>();
	stop->mode = ECamMode::STOP;
	event.commands.push_back(stop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

	m_nextState = EPlayerState::REPAIR_SHOP;

	CPlayerState::Enter();
}

void CPlayer_RepairShop::Exit()
{
	Evt_EndState e = {};
	//e.playerstate = E_PLAYERSTATE::VILLAGE;
	CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

	CPlayerState::Exit();
}

HRESULT CPlayer_RepairShop::Init_State()
{

	m_dialogueList.push_back("Repair_First");

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

int CPlayer_RepairShop::Update_State(const _float& timeDelta)
{

	if(m_inputManager->KeyDown(DIK_X))
	{

		return  ETOI(EPlayerState::VILLAGE);
	}

	else if (m_inputManager->KeyDown(DIK_F) && !m_itemIDInst.itemInstID == kIDAbsence)
	{
		if (m_itemIDInst.itemType == EItemType::EQUIP)
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


		}

	}
	else if (m_inputManager->MouseDown(EMouseButton::WHEEL))
	{
		Evt_MoveToStorage e = {};

		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);


	}
	return ETOI(m_nextState);
}

void CPlayer_RepairShop::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_RepairShop::Render_State()
{
	
}

shared_ptr<CPlayer_RepairShop> CPlayer_RepairShop::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> stateMachine)
{
	shared_ptr<CPlayer_RepairShop> instance(new CPlayer_RepairShop(owner,stateMachine), [](CPlayer_RepairShop* p) {p->Free(); delete(p); });


	return instance;
}

void CPlayer_RepairShop::Free()
{
	CPlayerState::Free();
}
