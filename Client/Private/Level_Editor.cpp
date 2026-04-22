#include "Client_Define.h"
//#include"Engine_Define.h"
#include "Level_Editor.h"

#include "Camera_Free.h"
#include "GameInstance.h"
#include "EventBus.h"
#include "Level_Loading.h"
#include "Event_Struct.h"

CLevel_Editor::CLevel_Editor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Editor::Initialize()
{



	CGameInstance::GetInstance()->Get_EventBus()->Subscribe<EvtCloneProto>([this](const EvtCloneProto& e)
		{
			if (CGameInstance::GetInstance()->Get_Current_LevelIdx() == ETOI(LEVEL::EDITOR)) 
			{
				m_pGameInstance.lock()->Add_GameObject(e.iLevel, e.Tag,
					ETOI(LEVEL::EDITOR), L"GameObject",nullptr);
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

void CLevel_Editor::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOGO))))
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

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;

	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (nullptr==(m_pGameInstance.lock()->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Camera(const _wstring& strLayerTag)
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

shared_ptr<CLevel_Editor> CLevel_Editor::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_Editor> pInstance(new CLevel_Editor(pDevice, pContext), [](CLevel_Editor* p) {p->Free();delete p;});

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Editor");
	}
	return pInstance;
}

void CLevel_Editor::Free()
{
	__super::Free();
}
