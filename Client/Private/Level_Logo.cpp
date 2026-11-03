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

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CLevel{ device, context }
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
	
	auto nameView = magic_enum::enum_name(EUiType::Test);
	_wstring wName = S2W(string(nameView));


	m_gameInstance.lock()->Load(ESaveType::UI, "MINI_GAME_5.json");



	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_ChangeLevel>([this](const Evt_ChangeLevel& e)
		{
			/*auto pLoadingUI = dynamic_pointer_cast<CUIRenderable>(CUI_Controller::GetInstance()->Get_LoadingUI());
			
			pLoadingUI->Set_Alpha(1.f);

			shared_ptr<CFadeModifier> eff = make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, .2f, false, _float4{ 0.f,0.f,0.f,0.f });
			pLoadingUI->m_behavior.push_back(eff);*/

		m_isChangeNextLevel = e.isChange;
			m_nextLevel = static_cast<ELevel>(e.level);

			m_acc = 0.f;
		});


	
		
	return S_OK;
}

HRESULT CLevel_Logo::Post_Initialize()
{
	m_gameInstance.lock()->UI_Push(EUILayer::WINDOW, L"MainMenu",true, nullptr);
	return S_OK;
}

void CLevel_Logo::Update(_float timeDelta)
{


	m_acc += timeDelta;
	if (m_isFlag == false)
	{
		CGameInstance::GetInstance()->Change_Camera(L"STOP_CAM");
		m_gameInstance.lock()->Load(ESaveType::GAMEOBJECT, "Lasst_2.json");
		m_isFlag = true;
	}

	if (m_gameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_RETURN) == true)
	{
		CUI_Controller::GetInstance()->Get_LoadingUI()->m_behavior.push_back(
			make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, .5f, false, _float4{ 0.f,0.f,0.f,0.f }));
		//for (auto& pChild : CUI_Controller::GetInstance()->Get_LoadingUI()->GetChildren())
		//{
		//	pChild->m_behavior.push_back(
		//		make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .5f, false, _float4{ 0.f,0.f,0.f,0.f }));
		//}
		m_gameInstance.lock()->Play_Loop(L"MainMenu_BGM");
	}
	

	if(GetKeyState(VK_SPACE) & 0x8000)
	{
	

		if (FAILED(m_gameInstance.lock()->Change_Level(ETOI(ELevel::LOADING), CLevel_Loading::Create(m_device, m_context, ELevel::GAMEPLAY))))
			return;
	}

	if (m_isChangeNextLevel == true&& m_acc >= 1.f)
	{
		m_gameInstance.lock()->Change_Level(ETOI(ELevel::LOADING), CLevel_Loading::Create(m_device, m_context, m_nextLevel));
		m_isChangeNextLevel = false;
		m_acc = 0;
	}

}

HRESULT CLevel_Logo::Ready_Layer_Camera(const wchar_t* str)
{
	CCamera_Free::CameraFreeDesc FRCamDesc = {};
	FRCamDesc.farZ = 500.f;
	FRCamDesc.nearZ = 0.1f;
	FRCamDesc.fovY = XMConvertToRadians(60.f);
	FRCamDesc.eyes = _float4(25.f, 9.f, 2.37f, 1.f);
	FRCamDesc.at = _float4(0.f, 0.f, 0.f, 1.f);
	FRCamDesc.speedPerSec = 10.f;
	FRCamDesc.degreePerSec = 180.f;
	FRCamDesc.mouseSensor = 0.01f;

	shared_ptr<CCamera> freeCamera = dynamic_pointer_cast<CCamera>(m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ETOI(ELevel::LOGO), str, &FRCamDesc));

	if (nullptr == freeCamera)
		return E_FAIL;
	m_gameInstance.lock()->Add_Camera(ETOI(ELevel::LOGO), L"STOP_CAM", freeCamera);

	freeCamera->Get_TransformCom()->Set_RotationDegree(_float3{-7.55f,33.0f, 0.f});

	dynamic_pointer_cast<CCamera_Free>(freeCamera)->Set_Stop(true);

}

HRESULT CLevel_Logo::Ready_Layer_Wave(const wchar_t* str)
{
	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Sea"),
		ETOI(ELevel::LOGO), str)))
		return E_FAIL;
	return S_OK;

}

HRESULT CLevel_Logo::Ready_Lights()
{

	LightDesc lightDesc{};

	lightDesc.type = ELightType::DIRECTIONAL;

	lightDesc.direction = _float4(1.f, -1.f, 1.f, 0.f);
	lightDesc.diffuse = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.ambient = _float4(0.2f, 0.2f, 0.2f, 1.f);
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

HRESULT CLevel_Logo::Render()
{

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("현재 로고레벨 구동 중"));
#endif
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& layerTag)
{
	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Terrain"),
		ETOI(ELevel::LOGO), layerTag)))
		return E_FAIL;





	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Sky"),
		ETOI(ELevel::LOGO), layerTag)))
		return E_FAIL;


	return S_OK;

}

shared_ptr<CLevel_Logo> CLevel_Logo::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CLevel_Logo> instance ( new CLevel_Logo(device, context));

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
	}
	return instance;
}

void CLevel_Logo::Free()
{
	//m_pGameInstance.lock()->UI_Detach_All();
	__super::Free();
}
