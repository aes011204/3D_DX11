#include "Level_GamePlay.h"

#include "Box.h"
#include "GameInstance.h"
#include "PlayerBoat.h"
#include "Level_Loading.h"
#include "Camera_Play.h"
#include "Camera_Free.h"
#include "DInput_Manager.h"
#include "FadeModifier.h"
#include "Fish.h"
#include "UI_TabContainer.h"
#include "UI_TabContainer.h"
#include "Inventory_Controller.h"
#include "Sea_Manager.h"
#include "UI_Item.h"
#include "UI_MiniGame.h"
#include "UI_NPC.h"
#include "UI_Village.h"
#include "UI_Controller.h"
#include "EventBus.h"
#include "ItemInfo.h"
#include "MiniGameController.h"
#include "Sky_Controller.h"
#include "UI_Time.h"
#include "WaterEff.h"


CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{


	CSky_Controller::GetInstance()->Initialize(m_device, m_context);
	auto Sky = CSky_Controller::GetInstance();
	if (Sky != nullptr)
	{
		
		m_gameInstance.lock()->Push_ManagerClass(L"SKY_CONTROLLER", Sky.get());
	}

	//CLog_Manager::GetInstance()->Add_Log(LOG_LEVEL::INFO, "senechangedII");
	//CLog_Manager::GetInstance()->Add_Log(LOG_LEVEL::WARNING, "senechangedWW");
	//CLog_Manager::GetInstance()->Add_Log(LOG_LEVEL::ERR, "senechangedEE");

	

	if (FAILED(Ready_Lights()))
		return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	//m_pPlayer.lock()->SetCam

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Moster"))))
		return E_FAIL;


	if (FAILED(Ready_Layer_Wave(TEXT("Layer_Wave"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_ETC(TEXT("Layer_ETC"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	//m_pGameInstance.lock()->Load(SAVETYPE::GAMEOBJECT, "Lasst_1.json");
	m_gameInstance.lock()->Load(ESaveType::UI, "MINI_GAME_5.json");

	


	return S_OK;
}

HRESULT CLevel_GamePlay::Post_Initialize()
{
	m_gameInstance.lock()->UI_Push(EUILayer::WINDOW, L"TabContainer", false , nullptr);
	m_tabUi = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"TabContainer");

	m_gameInstance.lock()->UI_Push(EUILayer::WINDOW, L"repairShop", false, nullptr);



	m_gameInstance.lock()->UI_Push(EUILayer::HUD, L"HUD", true, nullptr);



	m_gameInstance.lock()->UI_Push(EUILayer::WINDOW, L"NPC_Panel", false, nullptr);
	m_npc = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"NPC_Panel");

	m_gameInstance.lock()->UI_Push(EUILayer::OVERRIDE, L"ToolTip", false, nullptr);
	auto ToolTip = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"ToolTip");
	auto ui = dynamic_pointer_cast<CItemInfo>(ToolTip);
	ui->UI_InActive();

	m_gameInstance.lock()->UI_Push(EUILayer::OVERRIDE, L"HoldItem", false, nullptr);
	m_holdItem = dynamic_pointer_cast<CUI_Item>(m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::OVERRIDE, L"HoldItem"));


	m_gameInstance.lock()->UI_Push(EUILayer::WINDOW, L"MiniGame", false, nullptr);
	m_miniGame = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");

	//CUI_Controller::GetInstance()->Set_InvenCtrl(m_pInvenCntl);

	m_gameInstance.lock()->UI_Push(EUILayer::WINDOW, L"Village", false, nullptr);
	//m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");



	m_invenCtrl = CInventory_Controller::Create(m_device, m_context, m_playerInven, m_holdItem);
	m_miniGameController = CMiniGameController::Create();

	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_FishingData>([this](const Evt_FishingData e)
		{
			m_miniGameController->Start_Fishing(e.Fish, m_invenCtrl, e.RodSpeed);
		});


	m_gameInstance.lock()->UI_Push(EUILayer::WINDOW, L"Box", false, nullptr);
	//m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	
	m_gameInstance.lock()->UI_Push(EUILayer::STACK, L"Time", false, nullptr);

	m_time = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::STACK, L"Time");


	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	auto pGameInstance = m_gameInstance.lock();
	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		if (FAILED(pGameInstance->Change_Level(ETOI(ELevel::LOADING), CLevel_Loading::Create(m_device, m_context, ELevel::LOGO))))
			return;
	}


	if(m_isFlag ==false)
	{
		CGameInstance::GetInstance()->Change_Camera(L"Client_CAM");
		m_gameInstance.lock()->Load(ESaveType::GAMEOBJECT, "Lasst_2.json");
	//ClientCamera->Get_TransformCom()->Set_RotationDegree(_float3{ -12.44f,-1.98f, 0.f });
		m_isFlag = true;
	}

	if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_RETURN) == true)
	{
		CUI_Controller::GetInstance()->Get_LoadingUI()->m_behavior.push_back(
			make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, .5f, false, _float4{ 0.f,0.f,0.f,0.f }));
		//for (auto& pChild : CUI_Controller::GetInstance()->Get_LoadingUI()->GetChildren())
		//{
		//	pChild->m_behavior.push_back(
		//		make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .5f, false, _float4{ 0.f,0.f,0.f,0.f }));
		//}
		m_gameInstance.lock()->Play_Loop(L"GM_BGM_Theme");
		m_gameInstance.lock()->Play_Loop(L"GM_BGM_Night");
	}
	_bool CurIsNight = m_gameInstance.lock()->Get_IsNight();
	

	if(m_prevIsNight != CurIsNight)
	{
		if(m_gameInstance.lock()->Get_IsNight() == true)
		{
			m_gameInstance.lock()->Stop(L"GM_BGM_Day",1.5f);
			m_gameInstance.lock()->Play_Loop(L"GM_BGM_Night");

		}
		else
		{
			m_gameInstance.lock()->Play_Loop(L"GM_BGM_Day");
			m_gameInstance.lock()->Stop(L"GM_BGM_Night", 1.5f);

		}

		m_prevIsNight = CurIsNight;
	}
	



	if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_I))
	{
		if (nullptr == (pGameInstance->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_R"),
			ETOI(ELevel::GAMEPLAY), L"Layer_Moster")))
			return ;
	}
	if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_P))
	{
		if (nullptr == (pGameInstance->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Tentacle"),
			ETOI(ELevel::GAMEPLAY), L"Layer_Moster")))
			return ;
	}
	if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_O))
	{
		if (nullptr == (pGameInstance->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_MonkFish"),
			ETOI(ELevel::GAMEPLAY), L"Layer_Moster")))
			return ;
	}

	//if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_U))
	//{
	//	if (nullptr == (pGameInstance->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_R_Act"),
	//		ETOI(LEVEL::GAMEPLAY), L"Layer_Moster")))
	//		return;
	//}

	if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_U))
	{
		if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
			ETOI(ELevel::GAMEPLAY), L"Layer_Effect")))
			return;


	}






	if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_M))
	{
		dynamic_pointer_cast<CUI_Time>(m_time)->UI_Active();
	}
	if (pGameInstance->Get_DInput_Manger()->KeyDown(DIK_N))
	{
		dynamic_pointer_cast<CUI_Time>(m_time)->UI_InActive();
	}

	m_invenCtrl->Update(fTimeDelta);
	CSky_Controller::GetInstance()->Update(fTimeDelta);

}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("���� �����÷��� ���� ���� ��"));
#endif
	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Lights()
{

	m_gameInstance.lock()->Clear_Light();


	LightDesc lightDesc{};

	lightDesc.type = ELightType::DIRECTIONAL;

	lightDesc.direction = _float4(1.f, -1.f, 1.f, 0.f);
	lightDesc.diffuse = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.ambient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	lightDesc.specular = _float4(1.f, 1.f, 1.f, 1.f);

	if (nullptr==(m_gameInstance.lock()->Add_Light(lightDesc)))
		return E_FAIL;



	lightDesc.type = ELightType::POINT;
	lightDesc.position = _float4(0.f, 16.0f, 2.4f, 1.f);
	lightDesc.range = 20.f;
	lightDesc.diffuse = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.ambient = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.specular = _float4(1.f, 1.f, 1.f, 1.f);
	
	if (nullptr == (m_gameInstance.lock()->Add_Light(lightDesc)))
		return E_FAIL;

	lightDesc.type = ELightType::POINT;
	lightDesc.position = _float4(35.f, 12.3f, 17.f, 1.f);
	lightDesc.range = 15.f;
	lightDesc.diffuse = _float4(1.0f, 0.9f, 0.6f, 1.f);
	lightDesc.ambient = _float4(0.f, 0.f, 0.f, 1.f);
	lightDesc.specular = _float4(1.f, 1.f, 1.f, 1.f);
	if (nullptr == (m_gameInstance.lock()->Add_Light(lightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Terrain"),
		ETOI(ELevel::GAMEPLAY), strLayerTag)))
		return E_FAIL;



	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Sky"),
		ETOI(ELevel::GAMEPLAY), strLayerTag )))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{

	CCamera_Play::CameraPlayDesc CLCamDesc = {};
	CLCamDesc.farZ = 500.f;
	CLCamDesc.nearZ = 0.1f;
	CLCamDesc.fovY = XMConvertToRadians(60.f);
	CLCamDesc.eyes = _float4(34.48f, 6.587f, 6.038f, 1.f);
	CLCamDesc.at = _float4(34.13f, 8.43f, 15.80f, 1.f);
	//CLCamDesc.vDir = _float3(-12.44f, -1.98f, 0.f);
	CLCamDesc.speedPerSec = 10.f;
	CLCamDesc.degreePerSec = 180.f;
	CLCamDesc.mouseSensor = 0.05f;
	CLCamDesc.target = m_player;

	ClientCamera = dynamic_pointer_cast<CCamera>(m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Camera_Play"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &CLCamDesc));

	if (nullptr == ClientCamera)
		return E_FAIL;
	//ClientCamera->Get_TransformCom()->Set_RotationDegree(_float3{ -12.44f,-1.98f, 0.f });
	m_gameInstance.lock()->Add_Camera(ETOI(ELevel::GAMEPLAY), L"Client_CAM", ClientCamera);




	CCamera_Free::CameraFreeDesc FRCamDesc = {};
	FRCamDesc.farZ = 500.f;
	FRCamDesc.nearZ = 0.1f;
	FRCamDesc.fovY = XMConvertToRadians(60.f);
	FRCamDesc.eyes = _float4(34.48f,6.587f,6.038f,1.f);
	FRCamDesc.at = _float4(0.f, 0.f, 0.f, 1.f);
	FRCamDesc.speedPerSec = 10.f;
	FRCamDesc.degreePerSec = 180.f;
	FRCamDesc.mouseSensor = 0.01f;

	shared_ptr<CCamera> freeCamera = dynamic_pointer_cast<CCamera>(m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &FRCamDesc));

	if (nullptr == freeCamera)
		return E_FAIL;
	m_gameInstance.lock()->Add_Camera(ETOI(ELevel::GAMEPLAY), L"FREE_CAM", freeCamera);

	

//	CGameInstance::GetInstance()->Change_Camera(L"Client_CAM");
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
		ETOI(ELevel::GAMEPLAY), strLayerTag)))
		return E_FAIL;


	//if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Anim"),
	//	ETOI(LEVEL::GAMEPLAY), strLayerTag)))
	//	return E_FAIL;


	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Box"),
		ETOI(ELevel::GAMEPLAY), strLayerTag)))
		return E_FAIL;





	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{

	m_player = m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_PlayerBoat"),
		ETOI(ELevel::GAMEPLAY), strLayerTag);
	if (nullptr == m_player.lock())
		return E_FAIL;


	m_playerInven = dynamic_pointer_cast<CInventory>(m_player.lock()->Get_Component(L"Com_Inven"));
	if (nullptr == m_playerInven.lock())
		return E_FAIL;

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Layer_Wave(const _wstring& strLayerTag)
{

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Sea"),
		ETOI(ELevel::GAMEPLAY), strLayerTag)))
		return E_FAIL;
	return S_OK;

}
HRESULT CLevel_GamePlay::Ready_Layer_ETC(const _wstring& strLayerTag)
{
	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Village"),
		ETOI(ELevel::GAMEPLAY), strLayerTag)))
		return E_FAIL;
	


	CFish::FishDesc fishDesc = {};

	fishDesc.fish_DefID = 1001;
	fishDesc.FishCount = 4;
	fishDesc.Size = _float2(0.1f, .2f);
	fishDesc.Height = _float2(-.1f, .1f);
	fishDesc.Radius = _float2(1.f, 2.f);
	fishDesc.AlphaTime = _float2(1.f, 2.f);
	fishDesc.Speed = _float2(1.f, 2.f);
	fishDesc.MiniGameType = EMiniGameType::BASIC_CIRCLE;

	fishDesc.position = _float3(30.f, -1.f, 5.f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc)))
		return E_FAIL;


	//
	CFish::FishDesc fishDesc2 = {};

	fishDesc2.fish_DefID = 1002;
	fishDesc2.FishCount = 3;
	fishDesc2.Size = _float2(0.1f, .2f);
	fishDesc2.Height = _float2(-.1f, .1f);
	fishDesc2.Radius = _float2(1.f, 2.f);
	fishDesc2.AlphaTime = _float2(1.f, 2.f);
	fishDesc2.Speed = _float2(1.f, 2.f);
	fishDesc2.MiniGameType = EMiniGameType::DIAMOND;

	fishDesc2.position = _float3(40.f, -1.f, 5.f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc2)))
		return E_FAIL;




     fishDesc = {};

	fishDesc.fish_DefID = 1001;
	fishDesc.FishCount = 2;
	fishDesc.Size = _float2(0.1f, .2f);
	fishDesc.Height = _float2(-.1f, .1f);
	fishDesc.Radius = _float2(1.f, 2.f);
	fishDesc.AlphaTime = _float2(1.f, 2.f);
	fishDesc.Speed = _float2(1.f, 2.f);
	fishDesc.MiniGameType = EMiniGameType::BASIC_CIRCLE;

	fishDesc.position = _float3(33.126f, -1.f, -2.6f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc)))
		return E_FAIL;




	fishDesc = {};

	fishDesc.fish_DefID = 1002;
	fishDesc.FishCount = 2;
	fishDesc.Size = _float2(0.1f, .2f);
	fishDesc.Height = _float2(-.1f, .1f);
	fishDesc.Radius = _float2(1.f, 2.f);
	fishDesc.AlphaTime = _float2(1.f, 2.f);
	fishDesc.Speed = _float2(1.f, 2.f);
	fishDesc.MiniGameType = EMiniGameType::DIAMOND;

	fishDesc.position = _float3(54.6, -1.f, -3.9f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc)))
		return E_FAIL;


	fishDesc = {};

	fishDesc.fish_DefID = 1002;
	fishDesc.FishCount = 2;
	fishDesc.Size = _float2(0.1f, .2f);
	fishDesc.Height = _float2(-.1f, .1f);
	fishDesc.Radius = _float2(1.f, 2.f);
	fishDesc.AlphaTime = _float2(1.f, 2.f);
	fishDesc.Speed = _float2(1.f, 2.f);
	fishDesc.MiniGameType = EMiniGameType::DIAMOND;

	fishDesc.position = _float3(70.6, -1.f, -12.9f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc)))
		return E_FAIL;



	CFish::FishDesc fishDesc3 = {};

	fishDesc3.fish_DefID = 1003;
	fishDesc3.FishCount = 2;
	fishDesc3.Size = _float2(0.3f, .4f);
	fishDesc3.Height = _float2(-.1f, .1f);
	fishDesc3.Radius = _float2(1.5f, 2.f);
	fishDesc3.AlphaTime = _float2(1.f, 2.f);
	fishDesc3.Speed = _float2(1.f, 2.f);
	fishDesc3.MiniGameType = EMiniGameType::BASIC_CIRCLE;

	fishDesc3.position = _float3(57.9f, -1.f, 9.5f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc3)))
		return E_FAIL;


	CFish::FishDesc fishDesc4 = {};

	fishDesc4.fish_DefID = 1004;
	fishDesc4.FishCount = 2;
	fishDesc4.Size = _float2(0.3f, .5f);
	fishDesc4.Height = _float2(-.1f, .1f);
	fishDesc4.Radius = _float2(1.5f, 2.f);
	fishDesc4.AlphaTime = _float2(1.f, 2.f);
	fishDesc4.Speed = _float2(1.f, 2.f);
	fishDesc4.MiniGameType = EMiniGameType::DIAMOND;

	fishDesc4.position = _float3(79.f, -1.f, -3.f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc4)))
		return E_FAIL;





	fishDesc = {};

	fishDesc.fish_DefID = 1003;
	fishDesc.FishCount = 1;
	fishDesc.Size = _float2(0.1f, .2f);
	fishDesc.Height = _float2(-.1f, .1f);
	fishDesc.Radius = _float2(1.f, 2.f);
	fishDesc.AlphaTime = _float2(1.f, 2.f);
	fishDesc.Speed = _float2(1.f, 2.f);
	fishDesc.MiniGameType = EMiniGameType::BASIC_CIRCLE;

	fishDesc.position = _float3(100.f, -1.f, -17.f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc)))
		return E_FAIL;


	fishDesc = {};

	fishDesc.fish_DefID = 1004;
	fishDesc.FishCount = 1;
	fishDesc.Size = _float2(0.1f, .2f);
	fishDesc.Height = _float2(-.1f, .1f);
	fishDesc.Radius = _float2(1.f, 2.f);
	fishDesc.AlphaTime = _float2(1.f, 2.f);
	fishDesc.Speed = _float2(1.f, 2.f);
	fishDesc.MiniGameType = EMiniGameType::DIAMOND;

	fishDesc.position = _float3(121.5f, -1.f, 12.f);

	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(ELevel::GAMEPLAY), strLayerTag, &fishDesc)))
		return E_FAIL;

















	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_ColliderObject"),
		ETOI(ELevel::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{

	//if (nullptr == ((m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
	//	ETOI(LEVEL::GAMEPLAY), strLayerTag))))
	//	return E_FAIL;

	CWaterEff::EffDesc eff = {};
	eff.type = EEffType::PLAYER;
	eff.Target = dynamic_pointer_cast<CPlayerBoat>(m_player.lock());

	auto Eff = ((m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_WaterEff"), ETOI(ELevel::GAMEPLAY), strLayerTag,& eff)));
	if (Eff==nullptr)
		return E_FAIL;
	
	return  S_OK;
}



shared_ptr<CLevel_GamePlay> CLevel_GamePlay::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_GamePlay> pInstance(new CLevel_GamePlay(pDevice, pContext), [](CLevel_GamePlay* p) {p->Free();delete(p);});

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{


	__super::Free();
}
