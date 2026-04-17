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
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContext;

	Ready_UI();


	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_Cam_Arrived>([this](const Evt_Cam_Arrived e)
	{
		if(!CDialogueDB::GetInstance()->Get_PendingDialogue().empty())
		{
			auto m_Npc = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC_Panel");
			dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_NPCActive(CDialogueDB::GetInstance()->Get_PendingDialogue().front(), CDialogueDB::GetInstance()->Get_PendingDialogue());
			m_PendingUIState = e.playerstate;
		}
		else
		{
			m_PendingUIState = e.playerstate;
			StateUI();
			
		}

	});
	
	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_Dialogue_Finish>([this](const Evt_Dialogue_Finish e)
		{
		if(m_PendingUIState ==E_PLAYERSTATE::VILLAGE)
		{
			auto m_Npc = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC_Panel");
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
			auto MiniGame = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"MiniGame");
			auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
			if (ui)
			{
				ui->SetMiniGameLogic(e.logic);
			}
		});


	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_ItemHovered>([this](const Evt_ItemHovered& e)
		{
			
			auto ToolTip = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"ToolTip");
			auto ui = dynamic_pointer_cast<CItemInfo>(ToolTip);

			if (e.locationState == LOCATIONSTATE::END)
				ui->UI_InActive();
			ui->UI_PanelActive(e.isHold, e.itemInst, m_PendingUIState);
		});



	return S_OK;
}
void CUI_Controller::End_StateUI()
{


	if (m_PendingUIState == E_PLAYERSTATE::VILLAGE)
	{
		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
		m_Village->UI_InActive();

		auto m_Npc = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();
	}

	if (m_PendingUIState == E_PLAYERSTATE::FISHING)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
		m_HoldItem->UI_InActive();

		auto MiniGame = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"MiniGame");
		auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
		ui->UI_InActive();
	}

	if (m_PendingUIState == E_PLAYERSTATE::FISH_SHOP)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
		m_HoldItem->UI_InActive();

		auto m_Npc = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();
	}

	if (m_PendingUIState == E_PLAYERSTATE::REPAIR_SHOP)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
		m_HoldItem->UI_InActive();

		auto m_Npc = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();
	}

	if (m_PendingUIState == E_PLAYERSTATE::STORAGE)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		(Tab)->UI_InActive();

		auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
		m_HoldItem->UI_InActive();

		auto m_Npc = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC_Panel");
		dynamic_pointer_cast<CUI_NPC>(m_Npc)->UI_InActive();

		auto Box = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_InActive();
	}

}

void CUI_Controller::StateUI()
{
	if (m_PendingUIState == E_PLAYERSTATE::VILLAGE)
	{
		auto m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
		m_Village->UI_Active();
	}

	if (m_PendingUIState == E_PLAYERSTATE::FISHING)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(TAB::INVEN), TAB::INVEN);

		auto m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));
		m_HoldItem->UI_Active();

		auto MiniGame = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"MiniGame");
		auto ui = dynamic_pointer_cast<CUI_MiniGame>(MiniGame);
		ui->UI_PanelActive();


		


	}

	if (m_PendingUIState == E_PLAYERSTATE::FISH_SHOP)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(TAB::INVEN) | ETOI(TAB::STORAGE), TAB::INVEN);

	}

	if (m_PendingUIState == E_PLAYERSTATE::REPAIR_SHOP)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(TAB::INVEN) | ETOI(TAB::STORAGE), TAB::INVEN);

	}

	if (m_PendingUIState == E_PLAYERSTATE::STORAGE)
	{
		auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
		dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(TAB::INVEN), TAB::INVEN);

		auto Box = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Box");
		dynamic_pointer_cast<CUI_Box>(Box)->UI_PanelActive(true);

	}
}


void CUI_Controller::Update(float TimeDelta)
{
}

HRESULT CUI_Controller::Ready_UI()
{


	{

		CUIPanel::UIPANEL_DESC LoadingDesc;
		LoadingDesc.IsFullScreen = true;
		LoadingDesc.TextureProtoName = L"Prototype_Component_Texture_Black";
		LoadingDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		LoadingDesc.IsFullScreen = true;
		LoadingDesc.NoDISTACH = true;
		shared_ptr<CUIPanel> pInstance = CUIPanel::Create(m_pDevice, m_pContext);
		pInstance->Initialize(&LoadingDesc);

		CUIImage::UIIMAGE_DESC LoadingIconDesc;
		LoadingIconDesc.vPivot = _float2{ 1.f,0.f };
		LoadingIconDesc.vAnchorPoint = _float2{ 1.f,0.f };
		LoadingIconDesc.vAnchoredPos = _float2{ -40.f,40.f };
		LoadingIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		LoadingIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		LoadingIconDesc.TextureProtoName = L"Prototype_Component_Texture_LoadingIcon";
		shared_ptr<CUIImage> pIcon = CUIImage::Create(m_pDevice, m_pContext);
		pIcon->Initialize(&LoadingIconDesc);
		pIcon->m_behavior.push_back(make_shared<CRotationModifier>(200.f));
		pInstance->Add_Child(pIcon, L"ICON_LOADING", false);


		CUIText::TEXT_DESC TextDesc;
		//TextDesc.TextColor = _float4{ 1.f,0.f };
		TextDesc.strFontTag = L"Noto_Sans_CJK_SC_32";
		TextDesc.strText = L"심해로부터";
		TextDesc.fontaline = CUIText::FONTALINE::DEFAULT;

		shared_ptr<CUIText> text = CUIText::Create(m_pDevice, m_pContext);
		text->Initialize(&TextDesc);
		pInstance->Add_Child(text, L"text", false);


		m_pGameInstance.lock()->UI_InsertToPool(L"Loading", pInstance);

		/// 저장 해 두기 모든 레벨에서 필요 할예정
		
		m_Loading = pInstance;

	}


	CUI_MainMenu::MAINMENU_DESC pDescPanel;
	pDescPanel.IsFullScreen = true;
	pDescPanel.IsTransparent = true;

	shared_ptr<CUI_MainMenu> pInstance = CUI_MainMenu::Create(m_pDevice, m_pContext);
	pInstance->Initialize(&pDescPanel);
	m_pGameInstance.lock()->UI_InsertToPool(L"MainMenu", pInstance);
	///
	///
	///
	CUI_TabContainer::TABCONTAINER_DESC pDescTap = {};
	shared_ptr<CUI_TabContainer> TabContainer = CUI_TabContainer::Create(m_pDevice, m_pContext);
	if (TabContainer == nullptr)
		return E_FAIL;
	TabContainer->Initialize(&pDescTap);
	m_pGameInstance.lock()->UI_InsertToPool(L"TabContainer", TabContainer);



	CUI_Item::ITEM_DESC pDescitem = {};
	//CUI_Item::ITEM_DESC pDescitem = {};
	shared_ptr<CUI_Item>  holdItem = CUI_Item::Create(m_pDevice, m_pContext);
	if (holdItem == nullptr)
		return E_FAIL;
	holdItem->Initialize(&pDescitem);
	m_pGameInstance.lock()->UI_InsertToPool(L"HoldItem", holdItem);

	////////////HUD///////////////
	CUI_HUD::HUD_DESC pDescHUD;
	pDescHUD.IsFullScreen = true;
	pDescHUD.IsTransparent = true;

	shared_ptr<CUI_HUD> HUD = CUI_HUD::Create(m_pDevice, m_pContext);
	HUD->Initialize(&pDescHUD);
	m_pGameInstance.lock()->UI_InsertToPool(L"HUD", HUD);


	/////////////NPC//////////////
	
	CUI_NPC::NPC_DESC pNPCDesc;
	pNPCDesc.IsFullScreen = true;
	pNPCDesc.IsTransparent = true;

	shared_ptr<CUI_NPC> NPC = CUI_NPC::Create(m_pDevice, m_pContext);
	NPC->Initialize(&pNPCDesc);
	m_pGameInstance.lock()->UI_InsertToPool(L"NPC_Panel", NPC);


	/////////////tooltip//////////////

	CItemInfo::ITEMINFO_DESC pInfoDesc;

	shared_ptr<CItemInfo> tooltip = CItemInfo::Create(m_pDevice, m_pContext);
	tooltip->Initialize(&pInfoDesc);
	m_pGameInstance.lock()->UI_InsertToPool(L"ToolTip", tooltip);
	

	/////////////MiniGame//////////////

	CUI_MiniGame::MINIGAEMEPANEL_DESC pMiniGameDesc;


	shared_ptr<CUI_MiniGame> MiniGame = CUI_MiniGame::Create(m_pDevice, m_pContext);
	MiniGame->Initialize(&pMiniGameDesc);
	m_pGameInstance.lock()->UI_InsertToPool(L"MiniGame", MiniGame);
	m_miniGame = MiniGame;

	/////////////Village//////////////

	CUI_MainMenu::MAINMENU_DESC VillagePanel;
	VillagePanel.IsFullScreen = true;
	VillagePanel.IsTransparent = true;

	shared_ptr<CUI_Village> Village = CUI_Village::Create(m_pDevice, m_pContext);
	Village->Initialize(&VillagePanel);
	m_pGameInstance.lock()->UI_InsertToPool(L"Village", Village);

	/////////////BOX//////////////

	CUI_Box::BOX_DESC pDescBox = {};
	shared_ptr<CUI_Box> Box = CUI_Box::Create(m_pDevice, m_pContext);
	if (TabContainer == nullptr)
		return E_FAIL;
	Box->Initialize(&pDescBox);
	m_pGameInstance.lock()->UI_InsertToPool(L"Box", Box);









	return S_OK;
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
