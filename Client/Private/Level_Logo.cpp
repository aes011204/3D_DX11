#include "Level_Logo.h"

#include "GameInstance.h"
#include "Client_Enum.h"
#include "Engine_Helper.h"

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))));

	auto name_view = magic_enum::enum_name(EUI::Test);
	_wstring Wname = S2W(string(name_view));

	return S_OK;
}

HRESULT CLevel_Logo::Post_Initialize()
{
	
	m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"MainMenu",true, nullptr);
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if(GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return;
	}
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
	/*if(nullptr==
	m_pGameInstance.lock()->Add_GameObject(ETOI(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		ETOI(LEVEL::LOGO), strLayerTag))
	{
		return E_FAIL;
	}
*/


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
