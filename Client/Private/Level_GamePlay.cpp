#include "Level_GamePlay.h"

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


CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{

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

	return S_OK;
}

HRESULT CLevel_GamePlay::Post_Initialize()
{
	m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"TabContainer", false , nullptr);
	m_TapUI = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");

	m_pGameInstance.lock()->UI_Push(UI_LAYER::OVERRIDE, L"HoldItem", false, nullptr);
 	m_HoldItem = dynamic_pointer_cast<CUI_Item>(m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem"));

	m_pInvenCntl = CInventory_Controller::Create(m_pDevice, m_pContext,m_PlayerInven ,m_HoldItem);

	m_pGameInstance.lock()->UI_Push(UI_LAYER::HUD, L"HUD", true, nullptr);

	m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"NPC_Panel", false, nullptr);
	m_pNPC = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"NPC_Panel");

	m_pGameInstance.lock()->UI_Push(UI_LAYER::OVERRIDE, L"ToolTip", false, nullptr);


	m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"MiniGame", false, nullptr);
	m_pMiniGame = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"MiniGame");

	CUI_Controller::GetInstance()->Set_InvenCtrl(m_pInvenCntl);

	m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"Village", false, nullptr);
	m_Village = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"Village");
	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOGO))))
			return;
	}



	if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_RETURN) == true)
	{
		CUI_Controller::GetInstance()->Get_LoadingUI()->m_behavior.push_back(
			make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .5f, false, _float4{ 0.f,0.f,0.f,0.f }));
		//for (auto& pChild : CUI_Controller::GetInstance()->Get_LoadingUI()->GetChildren())
		//{
		//	pChild->m_behavior.push_back(
		//		make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .5f, false, _float4{ 0.f,0.f,0.f,0.f }));
		//}

	}


	if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_P))
	{
		if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Tentacle"),
			ETOI(LEVEL::GAMEPLAY), L"Layer_Moster")))
			return ;
	}

	// �ϴ� ���� �ΰ� ���߿� �������� UIHander, UIController �� �̵�
	//if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_TAB)) // �ϴ� Ű�� ������ ��
	//{
	//	if (m_OnTab == false) // �����־��ٸ� �ѱ�
	//	{

	//		dynamic_pointer_cast<CUI_TabContainer>(m_TapUI)->UI_PanelActive(ETOI(TAB::INVEN) | ETOI(TAB::STORAGE),TAB::INVEN);
	//		m_HoldItem->UI_Active();
	//		m_OnTab = true;
	//	}
	//	else // �����־��ٸ� ����
	//	{
	//		m_TapUI->UI_InActive();
	//		m_HoldItem->UI_InActive();

	//		m_OnTab = false;
	//	}
	//}
	m_pInvenCntl->Update(fTimeDelta);

	// 일단 테스트
	//if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_M))
	//{
	//	
	//	auto ui = dynamic_pointer_cast<CUI_NPC>(m_pNPC);
	//	ui->UI_NPCActive("Mayer_start",);

	//}
	//if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_N))
	//{
	//
	//	auto ui = dynamic_pointer_cast<CUI_MiniGame>(m_pMiniGame);
	//	ui->UI_PanelActive(CUI_MiniGame::BASIC_CIRCLE,1002);
	//
	//}
	//if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_B))
	//{

	//	auto ui = dynamic_pointer_cast<CUI_Village>(m_Village);
	//	ui->UI_Active();

	//}

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

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;

	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance.lock()->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;



	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Sky"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{

	CCamera_Play::CAMERAPLAY_DESC CLCamDesc = {};
	CLCamDesc.fFar = 500.f;
	CLCamDesc.fNear = 0.1f;
	CLCamDesc.fFovY = XMConvertToRadians(60.f);
	CLCamDesc.vAt = { 60.f, 0.f, 60.f, 1.f };
	CLCamDesc.vEyes = { 60.f, 60.f, -30.f, 1.f };
	CLCamDesc.fSpeedPerSec = 10.f;
	CLCamDesc.fDegreePerSec = 180.f;
	CLCamDesc.fMouseSensor = 0.05f;
	CLCamDesc.target = m_pPlayer;

	shared_ptr<CCamera> ClientCamera = dynamic_pointer_cast<CCamera>(m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Play"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag, &CLCamDesc));

	if (nullptr == ClientCamera)
		return E_FAIL;
	m_pGameInstance.lock()->Add_Camera(ETOI(LEVEL::GAMEPLAY), L"Client_CAM", ClientCamera);




	CCamera_Free::CAMERAFREE_DESC FRCamDesc = {};
	FRCamDesc.fFar = 500.f;
	FRCamDesc.fNear = 0.1f;
	FRCamDesc.fFovY = XMConvertToRadians(60.f);
	FRCamDesc.vEyes = _float4(0.f, 10.f, -7.f, 1.f);
	FRCamDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	FRCamDesc.fSpeedPerSec = 10.f;
	FRCamDesc.fDegreePerSec = 180.f;
	FRCamDesc.fMouseSensor = 0.01f;

	shared_ptr<CCamera> freeCamera = dynamic_pointer_cast<CCamera>(m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag, &FRCamDesc));

	if (nullptr == freeCamera)
		return E_FAIL;
	m_pGameInstance.lock()->Add_Camera(ETOI(LEVEL::GAMEPLAY), L"FREE_CAM", freeCamera);



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;


	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Anim"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;


	//	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Tentacle"),
	//		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{

	m_pPlayer = m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PlayerBoat"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag);
	if (nullptr == m_pPlayer.lock())
		return E_FAIL;


	m_PlayerInven = dynamic_pointer_cast<CInventory>(m_pPlayer.lock()->Get_Component(L"Com_Inven"));
	if (nullptr == m_PlayerInven.lock())
		return E_FAIL;

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Layer_Wave(const _wstring& strLayerTag)
{

	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Sea"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;
	return S_OK;

}
HRESULT CLevel_GamePlay::Ready_Layer_ETC(const _wstring& strLayerTag)
{
	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Village"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;
	


	CFish::FISH_DESC fishDesc = {};

	fishDesc.fish_DefID = 1001;
	fishDesc.FishCount = 5;
	fishDesc.Size = _float2(0.1f, .2f);
	fishDesc.Height = _float2(-.1f, .1f);
	fishDesc.Radius = _float2(1.f, 2.f);
	fishDesc.AlphaTime = _float2(1.f, 2.f);
	fishDesc.Speed = _float2(1.f, 2.f);


	fishDesc.vPosition = _float3(10.f, -2.f, 0.f);

	fishDesc.InvenCtrl = m_pInvenCntl;


	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Fish"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag, &fishDesc)))
		return E_FAIL;


	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Snow"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	

	if (nullptr == ((m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Explosion"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag))))
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
