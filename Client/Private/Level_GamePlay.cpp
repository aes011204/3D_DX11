#include "Level_GamePlay.h"

#include <UI.h>

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Play.h"
#include "Camera_Free.h"
#include "DInput_Manager.h"



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

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Post_Initialize()
{
	m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"TabContainer", false , nullptr);
	m_TapUI = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{

	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOGO))))
			return;
	}

	// 일단 여기 두고 나중에 많아지면 UIHander, UIController 로 이동
	if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_TAB)) // 일단 키가 눌렸을 때
	{
		if (m_OnTab == false) // 꺼져있었다면 켜기
		{
			m_TapUI->UI_Active();
			m_OnTab = true;
		}
		else // 켜져있었다면 끄기
		{
			m_TapUI->UI_InActive();
			m_OnTab = false;
		}
	}

}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("현재 게임플레이 레벨 구동 중"));
#endif
	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Lights()
{

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;

	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbition = _float4(1.f, 1.f, 1.f, 1.f);
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
		ETOI(LEVEL::STATIC), strLayerTag, &FRCamDesc));

	if (nullptr == freeCamera)
		return E_FAIL;
	m_pGameInstance.lock()->Add_Camera(ETOI(LEVEL::STATIC), L"FREE_CAM", freeCamera);



	return S_OK;
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
