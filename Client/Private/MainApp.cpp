#include "MainApp.h"

#include "GameInstance.h"
#include "ImguiManager.h"
#include "Data_Manager.h"
#include "Level_Loading.h"

#include "Inventory.h"


CMainApp::CMainApp()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{

   // if (!CreateDeviceD3D(g_hWnd)) return E_FAIL; // 장치 초기화로 함

    /* 게임을 구동하기 위한 기초 초기화 작업을 수행한다 */

    CData_Manager::GetInstance()->Create();


    /* 엔진을 이용하기 위한 엔진 츠로젝트를 준비시킨다 */
    ENGINE_DESC EngineDesc{};
    EngineDesc.hWnd = g_hWnd;
    EngineDesc.eWinMode = WINMODE::WIN;
    EngineDesc.iViewportHeight = g_iWinSizeY;
    EngineDesc.iViewportWidth = g_iWinSizeX;

    if(FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    /* 게임의 시작을 위해 시작이 되는 레벨 할당과 동작을 시킨다 */
    if (FAILED((Ready_StartLevel(LEVEL::LOGO))))
        return E_FAIL;


    //test
    CInventory::Create();

    //Imgui- 젤 마지막에
    {
        CImguiManager::GetInstance()->Initialize(g_hWnd, m_pDevice, m_pContext);
    }

    return S_OK;
}

int CMainApp::Update(_float fTimeDelta)
{

    m_pGameInstance->Update_Engine(fTimeDelta);


    //Imgui
    {
        CImguiManager::GetInstance()->Update();
    }
    float fps = 1.f / fTimeDelta;
    ImGui::Text("TimDelta = %.5f | FPS = %.1f ", fTimeDelta, fps);

    return 0;
}



void CMainApp::LateUpdate()
{


}

HRESULT CMainApp::Render()
{

    if (FAILED(m_pGameInstance->Bind_BackBufferRenderTarget(g_hWnd))) // 이거 Clear_Buffers() 내부로 넣을수 있지만 일단 이렇게
        return E_FAIL;

    _float4 vClearColor = { 0.f,0.f, 1.f,1.f };
    if (FAILED(m_pGameInstance->Clear_Buffers(&vClearColor)))
        return E_FAIL;


    m_pGameInstance->Draw();


        
    //Imgui
    {
        CImguiManager::GetInstance()->Render();
    }

    m_pGameInstance->Present();

    return S_OK;
}

HRESULT CMainApp::Ready_StartLevel(LEVEL eStartLevelID)
{
    if (eStartLevelID == LEVEL::LOADING)
        return E_FAIL;
    if (FAILED((m_pGameInstance->Change_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID)))))
        return E_FAIL;


}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMainApp::Free()
{
    __super::Free();

  
    Safe_Release(m_pGameInstance);
    CData_Manager::GetInstance()->DestroyInstance();
    
}

