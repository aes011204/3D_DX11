#include "MainApp.h"

#include "GameInstance.h"
#include "Client_Define.h"

#include "Data_Manager.h"
#include "DialogueDB.h"
#include "Level_Loading.h"
#include "Inventory.h"

#include "EditorInstance.h"

CMainApp::CMainApp()
    : m_pGameInstance{ CGameInstance::GetInstance() },
    m_pEditorInstance{ CEditorInstance::GetInstance()}
{
}

CMainApp::~CMainApp()
{
    Free();
}

HRESULT CMainApp::Initialize()
{

   // if (!CreateDeviceD3D(g_hWnd)) return E_FAIL; // 장치 초기화로 함

    /* 게임을 구동하기 위한 기초 초기화 작업을 수행한다 */

    //CData_Manager::GetInstance()->Initialize();
    //CDialogueDB::GetInstance()->Ready_DialogueDB();


    /* 엔진을 이용하기 위한 엔진 츠로젝트를 준비시킨다 */
    ENGINE_DESC EngineDesc{};
    EngineDesc.hWnd = g_hWnd;
    EngineDesc.eWinMode = WINMODE::WIN;
    EngineDesc.iMaxLevelNum = ETOI(LEVEL::END);
    EngineDesc.iViewportHeight = g_iWinSizeY;
    EngineDesc.iViewportWidth = g_iWinSizeX;

    if(FAILED(m_pGameInstance.lock()->Initialize_Engine(EngineDesc, m_pDevice, m_pContext)))
        return E_FAIL;

    if (FAILED(m_pEditorInstance.lock()->Initialize_Editor(EngineDesc, m_pDevice, m_pContext)))
        return E_FAIL;

    ImGuiContext* imgContext =  m_pEditorInstance.lock()->GetContext();

    ImGui::SetCurrentContext(imgContext);


    m_pGameInstance.lock()->SetImguiContext(imgContext);

    /* 게임의 시작을 위해 시작이 되는 레벨 할당과 동작을 시킨다 */
    if (FAILED((Ready_StartLevel(LEVEL::LOGO))))
        return E_FAIL;


    //test


   // CInventory::Create();

    ////Imgui- 젤 마지막에
    //{
    //    CImguiManager::GetInstance()->Initialize(g_hWnd, m_pDevice, m_pContext);
    //}



    return S_OK;
}

int CMainApp::Update(_float fTimeDelta)
{

    m_pGameInstance.lock()->Update_Engine(fTimeDelta);



    m_pEditorInstance.lock()->Update_Editor(fTimeDelta);
    ////Imgui
    //{
    //    CImguiManager::GetInstance()->Update();
    //}
    //float fps = 1.f / fTimeDelta;
    //ImGui::Text("TimDelta = %.5f | FPS = %.1f ", fTimeDelta, fps);

    return 0;
}

HRESULT CMainApp::Render()
{

    if (FAILED(m_pGameInstance.lock()->Bind_BackBufferRenderTarget(g_hWnd))) // 이거 Clear_Buffers() 내부로 넣을수 있지만 일단 이렇게
        return E_FAIL;

    _float4 vClearColor = { 0.f,0.f, 1.f,1.f };
    if (FAILED(m_pGameInstance.lock()->Clear_Buffers(&vClearColor)))
        return E_FAIL;


    m_pGameInstance.lock()->Draw();


    m_pEditorInstance.lock()->Render_Editor();

    ////Imgui
    //{
    //    CImguiManager::GetInstance()->Render();
    //}

    m_pGameInstance.lock()->Present();

    return S_OK;
}

HRESULT CMainApp::Ready_StartLevel(LEVEL eStartLevelID)
{
    if (eStartLevelID == LEVEL::LOADING)
        return E_FAIL;
    if (FAILED((m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID)))))
        return E_FAIL;

    return S_OK;
}

unique_ptr<CMainApp> CMainApp::Create()
{
    unique_ptr<CMainApp> pInstance ( new CMainApp());

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
    }
    return pInstance;
}

void CMainApp::Free()
{
    __super::Free();


    m_pGameInstance.lock()->DestroyInstance();
    m_pEditorInstance.lock()->DestroyInstance();

   // CData_Manager::GetInstance()->DestroyInstance();
    
}

