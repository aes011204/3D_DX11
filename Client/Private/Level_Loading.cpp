#include "Level_Loading.h"

#include <GameInstance.h>

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Editor.h"

//
#include "UIPanel.h"
#include "UI_Manager.h"
#include "UIButton.h"
#include "Client_Enum.h"
#include "Client_Helper.h"
#include "FadeModifier.h"
#include "DInput_Manager.h"
#include "UITransform.h"
#include "UI.h"
#include "GameInstance.h"

//

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_pGameInstance.lock()->Stop_All();
	m_eNextLevelID = eNextLevelID;
	
	// 로딩 화면을 구성해주기 위한 객체들을 생성한다
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	// 저 객체들이 로딩화면을 구성하고 유저에게 보여주는 동안 자원을 로드해줄수 있는 (Cloader)스레드 객체를 생성헤준다
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if(m_pLoader == nullptr)
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Loading::Post_Initialize()
{
	shared_ptr<CUI> lodingUI = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"Loading");
	if(lodingUI == nullptr)
	{
		m_pGameInstance.lock()->UI_Push(UI_LAYER::OVERRIDE, L"Loading", true, nullptr);
		

	}
	else
	{
		//lodingUI->UI_Active();
	}

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	//
	//m_LoadingUI->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT))
	//&& GetKeyState(VK_RETURN) & 0x8000

	if( m_pLoader->Is_Finished() == true)
	{


		shared_ptr<CLevel> pNextLevel = { nullptr };


		
		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNextLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;

		case LEVEL::GAMEPLAY:
			pNextLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::EDITOR:
			pNextLevel = CLevel_Editor::Create(m_pDevice, m_pContext);
			break;
		default:
			break;
		}

		if(pNextLevel==nullptr)
		{
			MSG_BOX("Failed to Created : NextLevel");
			return;
		}

		if(FAILED(m_pGameInstance.lock()->Change_Level(ETOI(m_eNextLevelID),pNextLevel)))
		{
			MSG_BOX("Failed to Change : NextLevel");
			return;
		}

		return;
	}

	

}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
	m_pLoader->Print_LoadText();
#endif

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _tchar* pLayerTag)
{


	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI(const _tchar* LayerTag)
{
	//CUITransform::UITRANSFORM_DESC desc ;
	//// 임시로 UI매니져에 ui 등록

	//shared_ptr pInstance = CUIPanel::Create(m_pDevice, m_pContext);
	//pInstance->Initialize(&desc);
	//shared_ptr panel = CUIPanel::Create(m_pDevice, m_pContext);
	//panel->Initialize(&desc);
	//shared_ptr button = CUIButton::Create(m_pDevice, m_pContext);
	//button->Initialize(&desc);

	//pInstance->Add_Child(panel, false);
	//pInstance->Add_Child(button, false);
	//m_pGameInstance.lock()->UI_InsertToPool(EnumToWString(EUI::Test), pInstance);

	return S_OK;
}

shared_ptr<CLevel_Loading> CLevel_Loading::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelID)
{
	shared_ptr<CLevel_Loading> pInstance ( new CLevel_Loading(pDevice, pContext));

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

}
