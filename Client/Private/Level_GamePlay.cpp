#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Log_Manager.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Play.h"
#include "Camera_Free.h"


CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{

	//CLog_Manager::GetInstance()->Add_Log(CLog_Manager::LOG_LEVEL::INFO, "senechangedII");
	//CLog_Manager::GetInstance()->Add_Log(CLog_Manager::LOG_LEVEL::WARNING, "senechangedWW");
	//CLog_Manager::GetInstance()->Add_Log(CLog_Manager::LOG_LEVEL::ERR, "senechangedEE");


	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if(FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::EDITOR))))
			return ;
	}
	int a = 10;
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("현재 게임플레이 레벨 구동 중"));
#endif
	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (nullptr==(m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{

	//CCamera_Play::CAMERAPLAY_DESC pCamDesc = {};
	//pCamDesc.fFar = 500.f;
	//pCamDesc.fNear = 0.1f;
	//pCamDesc.fFovY = XMConvertToRadians(60.f);
	//pCamDesc.vAt = { 60.f, 0.f, 60.f, 1.f };
	//pCamDesc.vEyes = { 60.f, 60.f, -30.f, 1.f };
	//pCamDesc.fSpeedPerSec = 10.f;
	//pCamDesc.fDegreePerSec = 180.f;
	//pCamDesc.fMouseSensor = 0.05f;
	//
	//if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Play"),
	//	ETOI(LEVEL::GAMEPLAY), strLayerTag, &pCamDesc)))
	//	return E_FAIL;
	//

	CCamera_Free::CAMERAFREE_DESC pCamDesc = {};
	pCamDesc.fFar = 500.f;
	pCamDesc.fNear = 0.1f;
	pCamDesc.fFovY = XMConvertToRadians(60.f);
	pCamDesc.vEyes = _float4(0.f, 10.f, -7.f, 1.f);
	pCamDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	pCamDesc.fSpeedPerSec = 10.f;
	pCamDesc.fDegreePerSec = 180.f;
	pCamDesc.fMouseSensor = 0.01f;

	if (nullptr == (m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag, &pCamDesc)))
		return E_FAIL;


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
