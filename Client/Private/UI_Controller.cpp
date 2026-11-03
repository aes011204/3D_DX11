#include "UI_Controller.h"

#include <numbers>

#include "ItemInfo.h"
#include "RotationModifier.h"
#include "UI_HUD.h"

#include "UI_MainMenu.h"
#include "UI_TabContainer.h"
#include "UI_Item.h"
#include "UIText.h"
#include "UI_NPC.h"
#include "UI_MiniGame.h"
#include "UI_Village.h"
#include "EventBus.h"
#include "DialogueDB.h"
#include "UI_Box.h"
#include "UI_RepairShop.h"
#include "UI_Time.h"

IMPLEMENT_SINGLETON(CUI_Controller)

CUI_Controller::CUI_Controller()

{
}

CUI_Controller::~CUI_Controller()
{
	//Free();
}

HRESULT CUI_Controller::Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	m_gameInstance = CGameInstance::GetInstance();
	m_device = pDevice;
	m_context = pContext;

	Ready_UI();


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_CamArrived>([this](const Evt_CamArrived e)
	{
		if(!CDialogueDB::GetInstance()->Get_PendingDialogue().empty())
		{
			auto m_Npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
			dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_NPCActive(CDialogueDB::GetInstance()->Get_PendingDialogue().front(), CDialogueDB::GetInstance()->Get_PendingDialogue());
			m_pendingUiState = e.playerstate;
		}
		else
		{
			m_pendingUiState = e.playerstate;
			StateUI();
			
		}

	});
	
	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_DialogueFinish>([this](const Evt_DialogueFinish e)
		{
		if(m_pendingUiState ==EPlayerState::VILLAGE)
		{
			auto m_Npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
			dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();
			
		}
			StateUI();
		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_EndState>([this](const Evt_EndState e)
		{
			End_StateUI();
		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_BindMiniGameLogic>([this](const Evt_BindMiniGameLogic& e)
		{
			auto MiniGame = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");
			auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
			if (ui)
			{
				ui->SetMiniGameLogic(e.logic);
			}
		});


	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_ItemHovered>([this](const Evt_ItemHovered& e)
		{
			
			auto ToolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
			auto ui = dynamic_pointer_cast<CItemInfo>(ToolTip);

			CacheToolTip(e);

			if (e.locationState == ELocationState::END || e.itemInst.itemInstId == kIdAbsence)
			{
				ui->UI_InActive();
				return;
			}

			ui->UI_PanelActive(e.isHold, e.itemInst, m_pendingUiState, e.IsPlayer);
			



		});

	return S_OK;
}
void CUI_Controller::End_StateUI()
{


	if (m_pendingUiState == EPlayerState::VILLAGE)
	{
		auto m_Village = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Village");
		m_Village->UI_InActive();

		auto m_Npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();
	}

	if (m_pendingUiState == EPlayerState::FISHING)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto MiniGame = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");
		auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
		ui->UI_InActive();
	}

	if (m_pendingUiState == EPlayerState::FISH_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_Npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();
	}

	if (m_pendingUiState == EPlayerState::REPAIR_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_Npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();


		auto m_repair = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"repairShop");
		dynamic_pointer_cast<CUI_RepairShop>(m_repair)->UI_InActive();
	}

	if (m_pendingUiState == EPlayerState::STORAGE)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_Npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_InActive();
	}
	if (m_pendingUiState == EPlayerState::INTERACT)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_Npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_InActive();
	}
}

void CUI_Controller::StateUI()
{
	if (m_pendingUiState == EPlayerState::VILLAGE)
	{
		auto m_Village = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Village");
		m_Village->UI_Active();
	}

	if (m_pendingUiState == EPlayerState::FISHING)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto MiniGame = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");
		auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
		ui->UI_PanelActive();


		



	}

	if (m_pendingUiState == EPlayerState::FISH_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN) | ETOI(ETab::STORAGE), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();
	}

	if (m_pendingUiState == EPlayerState::REPAIR_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN) | ETOI(ETab::STORAGE), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto m_repair = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"repairShop");
		dynamic_pointer_cast<CUI_RepairShop>(m_repair)->UI_PanelActive();
	}

	if (m_pendingUiState == EPlayerState::STORAGE)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_PanelActive(true);
	}
	if (m_pendingUiState == EPlayerState::INTERACT)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_PanelActive(false);
	}
}


void CUI_Controller::Update(float TimeDelta)
{
}

HRESULT CUI_Controller::Ready_UI()
{


	{

		CUIPanel::UIPanelDesc LoadingDesc;
		LoadingDesc.isFullScreen = true;
		LoadingDesc.textureProtoName = L"Prototype_Component_Texture_Black";
		LoadingDesc.textureComLevel = ETOI(ELevel::STATIC);
		LoadingDesc.isFullScreen = true;
		LoadingDesc.isNoDetach = true;
		shared_ptr<CUIPanel> pInstance = CUIPanel::Create(m_device, m_context);
		pInstance->Initialize(&LoadingDesc);

		CUIImage::UIImageDesc LoadingIconDesc;
		LoadingIconDesc.pivot = _float2{ 1.f,0.f };
		LoadingIconDesc.anchorPoint = _float2{ 1.f,0.f };
		LoadingIconDesc.anchoredPos = _float2{ -40.f,40.f };
		LoadingIconDesc.textureComLevel = ETOI(ELevel::STATIC);
		LoadingIconDesc.textureComLevel = ETOI(ELevel::STATIC);
		LoadingIconDesc.textureProtoName = L"Prototype_Component_Texture_LoadingIcon";
		shared_ptr<CUIImage> pIcon = CUIImage::Create(m_device, m_context);
		pIcon->Initialize(&LoadingIconDesc);
		pIcon->m_behavior.push_back(make_shared<CRotationModifier>(200.f));
		pInstance->Add_Child(pIcon, L"ICON_LOADING", false);


		CUIText::TextDesc textDesc;
		//TextDesc.TextColor = _float4{ 1.f,0.f };
		textDesc.fontTag = L"Noto_Sans_CJK_SC_32";
		textDesc.text = L"심해로부터";
		textDesc.fontAlign = CUIText::EFontAlign::DEFAULT;

		shared_ptr<CUIText> text = CUIText::Create(m_device, m_context);
		text->Initialize(&textDesc);
		pInstance->Add_Child(text, L"text", false);


		m_gameInstance.lock()->UI_InsertToPool(L"Loading", pInstance);

		/// 저장 해 두기 모든 레벨에서 필요 할예정
		
		m_loading = pInstance;

	}


	CUI_MainMenu::MainMenuDesc pDescPanel;
	pDescPanel.isFullScreen = true;
	pDescPanel.isTransparent = true;

	shared_ptr<CUI_MainMenu> pInstance = CUI_MainMenu::Create(m_device, m_context);
	pInstance->Initialize(&pDescPanel);
	m_gameInstance.lock()->UI_InsertToPool(L"MainMenu", pInstance);
	///
	///
	///
	CUI_TabContainer::TabContainerDesc pDescTap = {};
	shared_ptr<CUI_TabContainer> TabContainer = CUI_TabContainer::Create(m_device, m_context);
	if (TabContainer == nullptr)
		return E_FAIL;
	TabContainer->Initialize(&pDescTap);
	m_gameInstance.lock()->UI_InsertToPool(L"TabContainer", TabContainer);



	CUI_Item::ItemDesc pDescitem = {};
	//CUI_Item::ITEM_DESC pDescitem = {};
	shared_ptr<CUI_Item>  holdItem = CUI_Item::Create(m_device, m_context);
	if (holdItem == nullptr)
		return E_FAIL;
	holdItem->Initialize(&pDescitem);
	m_gameInstance.lock()->UI_InsertToPool(L"HoldItem", holdItem);

	////////////HUD///////////////
	CUI_HUD::HudDesc pDescHUD;
	pDescHUD.isFullScreen = true;
	pDescHUD.isTransparent = true;

	shared_ptr<CUI_HUD> HUD = CUI_HUD::Create(m_device, m_context);
	HUD->Initialize(&pDescHUD);
	m_gameInstance.lock()->UI_InsertToPool(L"HUD", HUD);


	/////////////NPC//////////////
	
	CUI_NPC::NpcDesc pNPCDesc;
	pNPCDesc.isFullScreen = true;
	pNPCDesc.isTransparent = true;

	shared_ptr<CUI_NPC> NPC = CUI_NPC::Create(m_device, m_context);
	NPC->Initialize(&pNPCDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"NPC_Panel", NPC);


	/////////////tooltip//////////////

	CItemInfo::ItemInfoDesc pInfoDesc;

	shared_ptr<CItemInfo> tooltip = CItemInfo::Create(m_device, m_context);
	tooltip->Initialize(&pInfoDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"ToolTip", tooltip);
	

	/////////////MiniGame//////////////

	CUI_MiniGame::MiniGamePanelDesc pMiniGameDesc;


	shared_ptr<CUI_MiniGame> MiniGame = CUI_MiniGame::Create(m_device, m_context);
	MiniGame->Initialize(&pMiniGameDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"MiniGame", MiniGame);
	m_miniGame = MiniGame;

	/////////////Village//////////////

	CUI_MainMenu::MainMenuDesc VillagePanel;
	VillagePanel.isFullScreen = true;
	VillagePanel.isTransparent = true;

	shared_ptr<CUI_Village> Village = CUI_Village::Create(m_device, m_context);
	Village->Initialize(&VillagePanel);
	m_gameInstance.lock()->UI_InsertToPool(L"Village", Village);

	/////////////BOX//////////////

	CUI_Box::BoxDesc pDescBox = {};
	shared_ptr<CUI_Box> Box = CUI_Box::Create(m_device, m_context);
	if (TabContainer == nullptr)
		return E_FAIL;
	Box->Initialize(&pDescBox);
	m_gameInstance.lock()->UI_InsertToPool(L"Box", Box);

	/////////////shop//////////////

	CUI_RepairShop::RepairDesc pRepairDesc= {};
	shared_ptr<CUI_RepairShop> repair = CUI_RepairShop::Create(m_device, m_context);
	if (repair == nullptr)
		return E_FAIL;

	repair->Initialize(&pRepairDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"repairShop", repair);



	/////////////Time//////////////

	CUI_Time::UiTimeDesc pTimeDesc = {};
	shared_ptr<CUI_Time> time = CUI_Time::Create(m_device, m_context);
	if (time == nullptr)
		return E_FAIL;

	time->Initialize(&pTimeDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"Time", time);
	m_time = time;


	return S_OK;
}

void CUI_Controller::ActiveTime(_uint time ,  _bool isSleep)
{
	m_time->UIPannelActive(time, isSleep);
}

void CUI_Controller::InActiveTime()
{
	m_time->UI_InActive();
}
void CUI_Controller::CacheToolTip(const Evt_ItemHovered& e)
{
	if (e.locationState == ELocationState::END || e.itemInst.itemInstId == kIdAbsence)
	{
		m_toolTipRestore = {};
		return;
	}

	m_toolTipRestore.bValid = true;
	m_toolTipRestore.isHold = e.isHold;
	m_toolTipRestore.IsPlayer = e.IsPlayer;
	m_toolTipRestore.itemInst = e.itemInst;
}

void CUI_Controller::RestoreToolTip()
{
	if (m_toolTipRestore.bValid == false)
		return;

	if (m_toolTipRestore.itemInst.itemInstId == kIdAbsence)
		return;

	auto ToolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
	auto ui = dynamic_pointer_cast<CItemInfo>(ToolTip);
	if (ui == nullptr)
		return;
	//ui->UI_Active();
	ui->UI_PanelActive(
		m_toolTipRestore.isHold,
		m_toolTipRestore.itemInst,
		m_pendingUiState,
		m_toolTipRestore.IsPlayer
	);
}
void CUI_Controller::ActiveHover()
{
	auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
	m_holdItem->UI_Active();

	RestoreToolTip();

}

void CUI_Controller::InActiveHover()
{
	auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
	m_holdItem->UI_InActive();

	auto ToolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
	auto ui = dynamic_pointer_cast<CItemInfo>(ToolTip);
	ui->UI_InActive();

}

void CUI_Controller::CloseToolTip()
{
	m_toolTipRestore = {};

	auto ToolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
	auto ui = dynamic_pointer_cast<CItemInfo>(ToolTip);
	if (ui)
		ui->UI_InActive();
}

//void CUI_Controller::Set_InvenCtrl(shared_ptr<CInventory_Controller> invenCtrl)
//{
//	{
//		m_miniGame->Set_InvenCtrl(invenCtrl);
//	}
//}



void CUI_Controller::Free()
{
	CBase::Free();
}