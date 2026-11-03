#include "Player_RepairShop.h"

#include <UI.h>

#include "DialogueDB.h"
#include "DInput_Manager.h"
#include "PlayerBoat.h"
#include "Sea_Manager.h"
#include "EventBus.h"
#include "ItemDB.h"
#include "Inventory.h"

CPlayer_RepairShop::CPlayer_RepairShop(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_RepairShop::~CPlayer_RepairShop()
{
}

void CPlayer_RepairShop::Enter()
{
	CDialogueDB::GetInstance()->Set_PendingDialogue(m_VecDialogue);


	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CamLerpDesc>();
	pLerp->eMode = ECamMode::LERP;
	pLerp->vTargetPos = _float3(3.14f, 3.429f, 5.734f);
	pLerp->vTargetRot = _float3(5.58f, -145.63f, 0.f);
	pLerp->fDuration = 2.0f;
	pLerp->fFov = 30.f;
	pLerp->OnComplete = [this]() {
		Evt_CamArrived e = {};
		e.playerstate = EPlayerState::REPAIR_SHOP;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
	};
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CamDesc>();
	pStop->eMode = ECamMode::STOP;
	event.commands.push_back(pStop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

	eNextState = EPlayerState::REPAIR_SHOP;

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

	m_VecDialogue.push_back("Repair_First");

	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ChangeState>([this](const Evt_ChangeState& e)
		{
			eNextState = e.playerstate;
		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_ItemHovered>([this](const Evt_ItemHovered& e)
		{
			m_ItemIdInst = e.itemInst;
		});


	return CPlayerState::Init_State();
}

int CPlayer_RepairShop::Update_State(const _float& timeDelta)
{

	if(m_Input_Manager->KeyDown(DIK_X))
	{

		return  ETOI(EPlayerState::VILLAGE);
	}

	else if (m_Input_Manager->KeyDown(DIK_F) && !m_ItemIdInst.itemInstId == ID_Absence)
	{
		if (m_ItemIdInst.itemType == EItemType::EQUIP)
		{
			float money = {};
			if (m_ItemIdInst.isMutation == true)
				money = m_ItemIdInst.mutationCaching.mutCost;
			else
			{
				auto def = CItemDB::GetInstance()->GetItemByID(m_ItemIdInst.itemDefId).typeDef;

				money = get<FishDef>(def).cost;

			}

			m_Owner.lock()->Add_Money(money);
			m_Owner.lock()->GetInventory()->RemoveFrom_Inven(m_ItemIdInst.itemInstId);


		}

	}
	else if (m_Input_Manager->MouseDown(DIMB::WHEEL))
	{
		Evt_MoveToStorage e = {};

		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);


	}
	return ETOI(eNextState);
}

void CPlayer_RepairShop::LateUpdate_State(const _float& timeDelta)
{
	
}

void CPlayer_RepairShop::Render_State()
{
	
}

shared_ptr<CPlayer_RepairShop> CPlayer_RepairShop::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_RepairShop> pInstance(new CPlayer_RepairShop(owner,pStateMachine), [](CPlayer_RepairShop* p) {p->Free(); delete(p); });


	return pInstance;
}

void CPlayer_RepairShop::Free()
{
	CPlayerState::Free();
}
