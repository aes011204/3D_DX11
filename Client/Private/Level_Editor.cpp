#include "Client_Define.h"
//#include"Engine_Define.h"
#include "Level_Editor.h"
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
		MSG_BOX("Failed to Created : CLevel_GamePlay");
	}
	return pInstance;
}

void CLevel_Editor::Free()
{
	__super::Free();
}
