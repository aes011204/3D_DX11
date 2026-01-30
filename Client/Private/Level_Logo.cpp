#include "Level_Logo.h"

#include <GameInstance.h>

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))));

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if(GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GMAEPLAYE))))
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
	if(FAILED(
	m_pGameInstance->Add_GameObject(ETOI(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		ETOI(LEVEL::LOGO), strLayerTag)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
