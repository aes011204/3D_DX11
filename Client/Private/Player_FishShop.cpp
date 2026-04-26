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

CPlayer_FishShop::CPlayer_FishShop(shared_ptr<CPlayerBoat> owner, shared_ptr < CPlayerStateMachine> pStateMachine)
	: CPlayerState(owner, pStateMachine)
{
}

CPlayer_FishShop::~CPlayer_FishShop()
{
}

void CPlayer_FishShop::Enter()
{

	CDialogueDB::GetInstance()->Set_PendingDialogue(m_VecDialogue);

	Evt_ChangeCam event = {};
	auto pLerp = make_shared<CAM_LERP_DESC>();
	pLerp->eMode = CAM_MODE::LERP;
	pLerp->vTargetPos = _float3(0.688f, 2.814f, 1.268f);
	pLerp->vTargetRot = _float3(21.49f, -50.13f, 0.f);
	pLerp->fDuration = 1.0f;
	//pLerp->fFov = 30.f;
	pLerp->OnComplete = [this]() {
		Evt_Cam_Arrived e = {};
		e.playerstate = E_PLAYERSTATE::FISH_SHOP;
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);
		};
	event.commands.push_back(pLerp);

	auto pStop = make_shared<CAM_DESC>();
	pStop->eMode = CAM_MODE::STOP;
	event.commands.push_back(pStop);



	CGameInstance::GetInstance()->Get_EventBus()->Publish(event);
	eNextState = E_PLAYERSTATE::FISH_SHOP;


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
	m_VecDialogue.push_back("FishStore_First");

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

int CPlayer_FishShop::Update_State(const _float& timeDelta)
{

	if (m_Input_Manager->KeyDown(DIK_X))
	{

		return  ETOI(E_PLAYERSTATE::VILLAGE);
	}
	else if (m_Input_Manager->KeyDown(DIK_F) && !m_ItemIdInst.ItemInst_ID == ID_Absence)
	{
		if (m_ItemIdInst.ItemType == ITEM_TYPE::FISH)
		{
			float money = {};
			if (m_ItemIdInst.IsMutaion == true)
				money = m_ItemIdInst.MutaionCashing.MutCost;
			else
			{
				auto def = CItemDB::GetInstance()->GetItemByID(m_ItemIdInst.ItemDef_ID).TypeDef;

				money = get<Fish_Def>(def).Cost;

			}

			m_Owner.lock()->Add_Money(money);
			m_Owner.lock()->GetInventory()->RemoveFrom_Inven(m_ItemIdInst.ItemInst_ID);
			

		}

	}
	else if (m_Input_Manager->MouseDown(DIMB::WHEEL))
	{
		Evt_MoveTo_Storage e = {};
		
		CGameInstance::GetInstance()->Get_EventBus()->Publish(e);

		
	}

	return ETOI(eNextState);
}

void CPlayer_FishShop::LateUpdate_State(const _float& timeDelta)
{

}

void CPlayer_FishShop::Render_State()
{

}

shared_ptr<CPlayer_FishShop> CPlayer_FishShop::Create(shared_ptr<CPlayerBoat> owner, shared_ptr<CPlayerStateMachine> pStateMachine)
{
	shared_ptr<CPlayer_FishShop> pInstance(new CPlayer_FishShop(owner, pStateMachine), [](CPlayer_FishShop* p) {p->Free(); delete(p); });


	return pInstance;
}

void CPlayer_FishShop::Free()
{
	CPlayerState::Free();
}
