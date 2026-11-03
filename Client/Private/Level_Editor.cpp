#include "Client_Define.h"
//#include"Engine_Define.h"
#include "Level_Editor.h"

#include "Camera_Free.h"
#include "GameInstance.h"
#include "EventBus.h"
#include "Level_Loading.h"
#include "Event_Struct.h"

CLevel_Editor::CLevel_Editor(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CLevel(device, context)
{
}

HRESULT CLevel_Editor::Initialize()
{



	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<EvtCloneProto>([this](const EvtCloneProto& e)
		{
			if (CGameInstance::GetInstance()->Get_Current_LevelIdx() == ETOI(ELevel::EDITOR)) 
			{
				m_gameInstance.lock()->Add_GameObject(e.level, e.tag,
					ETOI(ELevel::EDITOR), L"GameObject",nullptr);
			}
		});

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;



	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;


    return S_OK;
}

void CLevel_Editor::Update(_float timeDelta)
{
	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		if (FAILED(m_gameInstance.lock()->Change_Level(ETOI(ELevel::LOADING), CLevel_Loading::Create(m_device, m_context, ELevel::LOGO))))
			return;
	}
	int a = 10;
	//if (FAILED(m_pInstance.lock()->Change_Level(ETOI(eLevel), CLevel_Loading::Create(m_pDevice, m_pContext, eLevel))
	//{
	//	MSG_BOX("Failed to Change : NextLevel");
	//	return;
	//}
}



HRESULT CLevel_Editor::Ready_Lights()
{

	LightDesc lightDesc{};

	lightDesc.type = ELightType::DIRECTIONAL;

	lightDesc.direction = _float4(1.f, -1.f, 1.f, 0.f);
	lightDesc.diffuse = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.ambient = _float4(1.f, 1.f, 1.f, 1.f);
	lightDesc.specular = _float4(1.f, 1.f, 1.f, 1.f);

	if (nullptr==(m_gameInstance.lock()->Add_Light(lightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_BackGround(const _wstring& layerTag)
{
	if (nullptr == (m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ETOI(ELevel::GAMEPLAY), layerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Camera(const _wstring& layerTag)
{

	/*CCamera_Play::CAMERAPLAY_DESC CLCamDesc = {};
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
	m_pGameInstance.lock()->Add_Camera(ETOI(LEVEL::GAMEPLAY), L"Client_CAM", ClientCamera);*/




	CCamera_Free::CameraFreeDesc FRCamDesc = {};
	FRCamDesc.farZ = 500.f;
	FRCamDesc.nearZ = 0.1f;
	FRCamDesc.fovY = XMConvertToRadians(60.f);
	FRCamDesc.eyes = _float4(0.f, 10.f, -7.f, 1.f);
	FRCamDesc.at = _float4(0.f, 0.f, 0.f, 1.f);
	FRCamDesc.speedPerSec = 10.f;
	FRCamDesc.degreePerSec = 180.f;
	FRCamDesc.mouseSensor = 0.01f;

	shared_ptr<CCamera> freeCamera = dynamic_pointer_cast<CCamera>(m_gameInstance.lock()->Add_GameObject(ETOI(ELevel::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ETOI(ELevel::STATIC), layerTag, &FRCamDesc));

	if (nullptr == freeCamera)
		return E_FAIL;
	m_gameInstance.lock()->Add_Camera(ETOI(ELevel::STATIC), L"FREE_CAM", freeCamera);



	return S_OK;
}


HRESULT CLevel_Editor::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("현재 에디터레벨 구동 중"));
#endif
    return S_OK;
}


void CLevel_Editor::OnGui()
{

}

shared_ptr<CLevel_Editor> CLevel_Editor::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CLevel_Editor> instance(new CLevel_Editor(device, context), [](CLevel_Editor* p) {p->Free();delete p;});

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Editor");
	}
	return instance;
}

void CLevel_Editor::Free()
{
	__super::Free();
}
