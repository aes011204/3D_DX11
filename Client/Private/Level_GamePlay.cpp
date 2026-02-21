#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Log_Manager.h"
#include "GameInstance.h"
#include "Level_Loading.h"


CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{

	CLog_Manager::GetInstance()->Add_Log(CLog_Manager::LOG_LEVEL::INFO, "senechangedII");
	CLog_Manager::GetInstance()->Add_Log(CLog_Manager::LOG_LEVEL::WARNING, "senechangedWW");
	CLog_Manager::GetInstance()->Add_Log(CLog_Manager::LOG_LEVEL::ERR, "senechangedEE");

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

shared_ptr<CLevel_GamePlay> CLevel_GamePlay::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_GamePlay> pInstance ( new CLevel_GamePlay(pDevice, pContext));

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
