#include "Level_Logo.h"

#include "GameInstance.h"
#include "Client_Enum.h"
#include "Engine_Helper.h"
#include "FadeModifier.h"
#include "UI_Controller.h"
#include "DInput_Manager.h"
#include "Camera_Free.h"
#include "UI.h"
#include "UIRenderable.h"

#include "Level_Loading.h"
#include "EventBus.h"

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}


HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))));
	
	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	
	//
	if (FAILED(Ready_Layer_Wave(TEXT("Layer_Wave"))))
		return E_FAIL;
	//
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
	auto name_view = magic_enum::enum_name(EUI::Test);
	_wstring Wname = S2W(string(name_view));


	m_pGameInstance.lock()->Load(SAVETYPE::UI, "MINI_GAME_5.json");



	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_ChangeLevel>([this](const Evt_ChangeLevel& e)
		{
			/*auto pLoadingUI = dynamic_pointer_cast<CUIRenderable>(CUI_Controller::GetInstance()->Get_LoadingUI());
			
			pLoadingUI->Set_Alpha(1.f);

			shared_ptr<CFadeModifier> eff = make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, .2f, false, _float4{ 0.f,0.f,0.f,0.f });
			pLoadingUI->m_behavior.push_back(eff);*/

		ChangeNextLevel = e.IsChange;
			m_NextLevel = static_cast<LEVEL>(e.level);

			m_Acc = 0.f;
		});


	
		
	return S_OK;
}

HRESULT CLevel_Logo::Post_Initialize()
{
	m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"MainMenu",true, nullptr);
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{


	m_Acc += fTimeDelta;
	if (m_Flag == false)
	{
		CGameInstance::GetInstance()->Change_Camera(L"STOP_CAM");
		m_pGameInstance.lock()->Load(SAVETYPE::GAMEOBJECT, "Lasst_2.json");
		m_Flag = true;
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
		m_pGameInstance.lock()->Play_Loop(L"MainMenu_BGM");
	}
	

	if(GetKeyState(VK_SPACE) & 0x8000)
	{
	

		if (FAILED(m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return;
	}

	if (ChangeNextLevel == true&& m_Acc >= 1.f)
	{
		m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, m_NextLevel));
		ChangeNextLevel = false;
		m_Acc = 0;
	}

}

HRESULT CLevel_Logo::Ready_Layer_Camera(const wchar_t* str)
{
	CCamera_Free::CAMERAFREE_DESC FRCamDesc = {};
	FRCamDesc.fFar = 500.f;
	FRCamDesc.fNear = 0.1f;
	FRCamDesc.fFovY = XMConvertToRadians(60.f);
	FRCamDesc.vEyes = _float4(25.f, 9.f, 2.37f, 1.f);
	FRCamDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	FRCamDesc.fSpeedPerSec = 10.f;
	FRCamDesc.fDegreePerSec = 180.f;
	FRCamDesc.fMouseSensor = 0.01f;

	shared_ptr<CCamera> freeCamera = dynamic_pointer_cast<CCamera>(m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ETOI(LEVEL::LOGO), str, &FRCamDesc));

	if (nullptr == freeCamera)
		return E_FAIL;
	m_pGameInstance.lock()->Add_Camera(ETOI(LEVEL::LOGO), L"STOP_CAM", freeCamera);

	freeCamera->Get_TransformCom()->Set_RotationDegree(_float3{-7.55f,33.0f, 0.f});

	dynamic_pointer_cast<CCamera_Free>(freeCamera)->Set_Stop(true);

}

HRESULT CLevel_Logo::Ready_Layer_Wave(const wchar_t* str)
{
	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Sea"),
		ETOI(LEVEL::LOGO), str)))
		return E_FAIL;
	return S_OK;

}

HRESULT CLevel_Logo::Ready_Lights()
{

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;

	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (nullptr == (m_pGameInstance.lock()->Add_Light(LightDesc)))
		return E_FAIL;

	

	LightDesc.eType = LIGHT::POINT;
	LightDesc.vPosition = _float4(35.f, 12.3f, 17.f, 1.f);
	LightDesc.fRange = 15.f;
	LightDesc.vDiffuse = _float4(1.0f, 0.9f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	if (nullptr == (m_pGameInstance.lock()->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Render()
{

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("현재 로고레벨 구동 중"));
#endif
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Terrain"),
		ETOI(LEVEL::LOGO), strLayerTag)))
		return E_FAIL;





	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Sky"),
		ETOI(LEVEL::LOGO), strLayerTag)))
		return E_FAIL;


	return S_OK;

}

shared_ptr<CLevel_Logo> CLevel_Logo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_Logo> pInstance ( new CLevel_Logo(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	//m_pGameInstance.lock()->UI_Detach_All();
	__super::Free();
}
