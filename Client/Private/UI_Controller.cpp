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

HRESULT CUI_Controller::Initialize(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	m_gameInstance = CGameInstance::GetInstance();
	m_device = device;
	m_context = context;

	Ready_UI();


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_CamArrived>([this](const Evt_CamArrived e)
	{
		if(!CDialogueDB::GetInstance()->Get_PendingDialogue().empty())
		{
			auto m_NPC = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
			dynamic_pointer_cast<CUI_NPC>(m_NPC)->UI_NPCActive(CDialogueDB::GetInstance()->Get_PendingDialogue().front(), CDialogueDB::GetInstance()->Get_PendingDialogue());
			m_pendingUIState = e.playerstate;
		}
		else
		{
			m_pendingUIState = e.playerstate;
			StateUI();
			
		}

	});
	
	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_DialogueFinish>([this](const Evt_DialogueFinish e)
		{
		if(m_pendingUIState ==EPlayerState::VILLAGE)
		{
			auto m_NPC = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
			dynamic_pointer_cast<CUI_NPC>(m_NPC)->UI_InActive();
			
		}
			StateUI();
		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_EndState>([this](const Evt_EndState e)
		{
			End_StateUI();
		});


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_BindMiniGameLogic>([this](const Evt_BindMiniGameLogic& e)
		{
			auto miniGame = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");
			auto UI = dynamic_pointer_cast<CUI_MiniGame>(miniGame);
			if (UI)
			{
				UI->SetMiniGameLogic(e.logic);
			}
		});


	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_ItemHovered>([this](const Evt_ItemHovered& e)
		{
			
			auto toolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
			auto UI = dynamic_pointer_cast<CItemInfo>(toolTip);

			CacheToolTip(e);

			if (e.locationState == ELocationState::END || e.itemInst.itemInstID == kIDAbsence)
			{
				UI->UI_InActive();
				return;
			}

			UI->UI_PanelActive(e.isHold, e.itemInst, m_pendingUIState, e.isPlayer);
			



		});

	return S_OK;
}
void CUI_Controller::End_StateUI()
{


	if (m_pendingUIState == EPlayerState::VILLAGE)
	{
		auto m_Village = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Village");
		m_Village->UI_InActive();

		auto m_NPC = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_NPC)->UI_InActive();
	}

	if (m_pendingUIState == EPlayerState::FISHING)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto miniGame = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");
		auto UI = dynamic_pointer_cast<CUI_MiniGame>(miniGame);
		UI->UI_InActive();
	}

	if (m_pendingUIState == EPlayerState::FISH_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_NPC = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_NPC)->UI_InActive();
	}

	if (m_pendingUIState == EPlayerState::REPAIR_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_NPC = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_NPC)->UI_InActive();


		auto m_repair = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"repairShop");
		dynamic_pointer_cast<CUI_RepairShop>(m_repair)->UI_InActive();
	}

	if (m_pendingUIState == EPlayerState::STORAGE)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_NPC = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_NPC)->UI_InActive();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_InActive();
	}
	if (m_pendingUIState == EPlayerState::INTERACT)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_InActive();

		auto m_NPC = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_NPC)->UI_InActive();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_InActive();
	}
}

void CUI_Controller::StateUI()
{
	if (m_pendingUIState == EPlayerState::VILLAGE)
	{
		auto m_Village = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Village");
		m_Village->UI_Active();
	}

	if (m_pendingUIState == EPlayerState::FISHING)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto miniGame = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");
		auto UI = dynamic_pointer_cast<CUI_MiniGame>(miniGame);
		UI->UI_PanelActive();


		



	}

	if (m_pendingUIState == EPlayerState::FISH_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN) | ETOI(ETab::STORAGE), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();
	}

	if (m_pendingUIState == EPlayerState::REPAIR_SHOP)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN) | ETOI(ETab::STORAGE), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto m_repair = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"repairShop");
		dynamic_pointer_cast<CUI_RepairShop>(m_repair)->UI_PanelActive();
	}

	if (m_pendingUIState == EPlayerState::STORAGE)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_PanelActive(true);
	}
	if (m_pendingUIState == EPlayerState::INTERACT)
	{
		auto Tab = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(ETab::INVEN), ETab::INVEN);

		auto m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));
		m_holdItem->UI_Active();

		auto Box = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_PanelActive(false);
	}
}


void CUI_Controller::Update(float timeDelta)
{
}

HRESULT CUI_Controller::Ready_UI()
{


	{

		CUIPanel::UIPanelDesc loadingDesc;
		loadingDesc.isFullScreen = true;
		loadingDesc.textureProtoName = L"Prototype_Component_Texture_Black";
		loadingDesc.textureComLevel = ETOI(ELevel::STATIC);
		loadingDesc.isFullScreen = true;
		loadingDesc.isNoDetach = true;
		shared_ptr<CUIPanel> instance = CUIPanel::Create(m_device, m_context);
		instance->Initialize(&loadingDesc);

		CUIImage::UIImageDesc loadingIconDesc;
		loadingIconDesc.pivot = _float2{ 1.f,0.f };
		loadingIconDesc.anchorPoint = _float2{ 1.f,0.f };
		loadingIconDesc.anchoredPos = _float2{ -40.f,40.f };
		loadingIconDesc.textureComLevel = ETOI(ELevel::STATIC);
		loadingIconDesc.textureComLevel = ETOI(ELevel::STATIC);
		loadingIconDesc.textureProtoName = L"Prototype_Component_Texture_LoadingIcon";
		shared_ptr<CUIImage> icon = CUIImage::Create(m_device, m_context);
		icon->Initialize(&loadingIconDesc);
		icon->m_behavior.push_back(make_shared<CRotationModifier>(200.f));
		instance->Add_Child(icon, L"ICON_LOADING", false);


		CUIText::TextDesc textDesc;
		//TextDesc.TextColor = _float4{ 1.f,0.f };
		textDesc.fontTag = L"Noto_Sans_CJK_SC_32";
		textDesc.text = L"심해로부터";
		textDesc.fontAlign = CUIText::EFontAlign::DEFAULT;

		shared_ptr<CUIText> text = CUIText::Create(m_device, m_context);
		text->Initialize(&textDesc);
		instance->Add_Child(text, L"text", false);


		m_gameInstance.lock()->UI_InsertToPool(L"Loading", instance);

		/// 저장 해 두기 모든 레벨에서 필요 할예정
		
		m_loading = instance;

	}


	CUI_MainMenu::MainMenuDesc descPanel;
	descPanel.isFullScreen = true;
	descPanel.isTransparent = true;

	shared_ptr<CUI_MainMenu> instance = CUI_MainMenu::Create(m_device, m_context);
	instance->Initialize(&descPanel);
	m_gameInstance.lock()->UI_InsertToPool(L"MainMenu", instance);
	///
	///
	///
	CUI_TabContainer::TabContainerDesc descTap = {};
	shared_ptr<CUI_TabContainer> tabContainer = CUI_TabContainer::Create(m_device, m_context);
	if (tabContainer == nullptr)
		return E_FAIL;
	tabContainer->Initialize(&descTap);
	m_gameInstance.lock()->UI_InsertToPool(L"TabContainer", tabContainer);



	CUI_Item::ItemDesc descitem = {};
	//CUI_Item::ITEM_DESC pDescitem = {};
	shared_ptr<CUI_Item>  holdItem = CUI_Item::Create(m_device, m_context);
	if (holdItem == nullptr)
		return E_FAIL;
	holdItem->Initialize(&descitem);
	m_gameInstance.lock()->UI_InsertToPool(L"HoldItem", holdItem);

	////////////HUD///////////////
	CUI_HUD::HUDDesc descHUD;
	descHUD.isFullScreen = true;
	descHUD.isTransparent = true;

	shared_ptr<CUI_HUD> HUD = CUI_HUD::Create(m_device, m_context);
	HUD->Initialize(&descHUD);
	m_gameInstance.lock()->UI_InsertToPool(L"HUD", HUD);


	/////////////NPC//////////////
	
	CUI_NPC::NPCDesc NPCDesc;
	NPCDesc.isFullScreen = true;
	NPCDesc.isTransparent = true;

	shared_ptr<CUI_NPC> NPC = CUI_NPC::Create(m_device, m_context);
	NPC->Initialize(&NPCDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"NPC_Panel", NPC);


	/////////////tooltip//////////////

	CItemInfo::ItemInfoDesc infoDesc;

	shared_ptr<CItemInfo> tooltip = CItemInfo::Create(m_device, m_context);
	tooltip->Initialize(&infoDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"ToolTip", tooltip);
	

	/////////////MiniGame//////////////

	CUI_MiniGame::MiniGamePanelDesc miniGameDesc;


	shared_ptr<CUI_MiniGame> miniGame = CUI_MiniGame::Create(m_device, m_context);
	miniGame->Initialize(&miniGameDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"MiniGame", miniGame);
	m_miniGame = miniGame;

	/////////////Village//////////////

	CUI_MainMenu::MainMenuDesc villagePanel;
	villagePanel.isFullScreen = true;
	villagePanel.isTransparent = true;

	shared_ptr<CUI_Village> village = CUI_Village::Create(m_device, m_context);
	village->Initialize(&villagePanel);
	m_gameInstance.lock()->UI_InsertToPool(L"Village", village);

	/////////////BOX//////////////

	CUI_Box::BoxDesc descBox = {};
	shared_ptr<CUI_Box> Box = CUI_Box::Create(m_device, m_context);
	if (tabContainer == nullptr)
		return E_FAIL;
	Box->Initialize(&descBox);
	m_gameInstance.lock()->UI_InsertToPool(L"Box", Box);

	/////////////shop//////////////

	CUI_RepairShop::RepairDesc repairDesc= {};
	shared_ptr<CUI_RepairShop> repair = CUI_RepairShop::Create(m_device, m_context);
	if (repair == nullptr)
		return E_FAIL;

	repair->Initialize(&repairDesc);
	m_gameInstance.lock()->UI_InsertToPool(L"repairShop", repair);



	/////////////Time//////////////

	CUI_Time::UITimeDesc timeDesc = {};
	shared_ptr<CUI_Time> time = CUI_Time::Create(m_device, m_context);
	if (time == nullptr)
		return E_FAIL;

	time->Initialize(&timeDesc);
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
	if (e.locationState == ELocationState::END || e.itemInst.itemInstID == kIDAbsence)
	{
		m_toolTipRestore = {};
		return;
	}

	m_toolTipRestore.isValid = true;
	m_toolTipRestore.isHold = e.isHold;
	m_toolTipRestore.isPlayer = e.isPlayer;
	m_toolTipRestore.itemInst = e.itemInst;
}

void CUI_Controller::RestoreToolTip()
{
	if (m_toolTipRestore.isValid == false)
		return;

	if (m_toolTipRestore.itemInst.itemInstID == kIDAbsence)
		return;

	auto toolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
	auto UI = dynamic_pointer_cast<CItemInfo>(toolTip);
	if (UI == nullptr)
		return;
	//ui->UI_Active();
	UI->UI_PanelActive(
		m_toolTipRestore.isHold,
		m_toolTipRestore.itemInst,
		m_pendingUIState,
		m_toolTipRestore.isPlayer
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

	auto toolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
	auto UI = dynamic_pointer_cast<CItemInfo>(toolTip);
	UI->UI_InActive();

}

void CUI_Controller::CloseToolTip()
{
	m_toolTipRestore = {};

	auto toolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
	auto UI = dynamic_pointer_cast<CItemInfo>(toolTip);
	if (UI)
		UI->UI_InActive();
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