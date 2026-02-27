#include "MainApp.h"

#include "GameInstance.h"
#include "Client_Define.h"

#include "Data_Manager.h"
#include "DialogueDB.h"
#include "Inventory.h"

#include "Level_Loading.h"
#include "EditorInstance.h"
#include "BackGround.h"
#include "UIButton.h"
#include "UICanvas.h"
#include "UIPanel.h"
#include "UIImage.h"
#include "CScaleModifier.h"
#include "IModifier.h"
#include "EmptyGameObject.h"
#include "Camera_Play.h"
#include "Camera_Free.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pEditorInstance{ CEditorInstance::GetInstance() }
{
}

CMainApp::~CMainApp()
{
	Free();
}

HRESULT CMainApp::Initialize()
{


	/* 게임을 구동하기 위한 기초 초기화 작업을 수행한다 */



	/* 엔진을 이용하기 위한 엔진 츠로젝트를 준비시킨다 */
	ENGINE_DESC EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.hInst = g_hInst;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iMaxLevelNum = ETOI(LEVEL::END);
	EngineDesc.iEditorLevel = ETOI(LEVEL::EDITOR);
	EngineDesc.iViewportHeight = g_iWinSizeY;
	EngineDesc.iViewportWidth = g_iWinSizeX;

	if (FAILED(m_pGameInstance.lock()->Initialize_Engine(EngineDesc, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(m_pEditorInstance.lock()->Initialize_Editor(EngineDesc, m_pDevice, m_pContext)))
		return E_FAIL;

	ImGuiContext* imgContext = m_pEditorInstance.lock()->GetContext();
	ImGui::SetCurrentContext(imgContext);
	m_pGameInstance.lock()->SetImguiContext(imgContext);


	/* 게임의 시작을 위해 시작이 되는 레벨 할당과 동작을 시킨다 */
	if (FAILED((Ready_StartLevel(LEVEL::LOGO))))
		return E_FAIL;

	if (FAILED((Ready_Prototype_For_Static_Level())))
		return E_FAIL;


	if (FAILED((Ready_UI())))
		return E_FAIL;

	//test
   // CInventory::Create();
	//CData_Manager::GetInstance()->Initialize();
	//CDialogueDB::GetInstance()->Ready_DialogueDB();





	return S_OK;
}

int CMainApp::Update(_float fTimeDelta)
{

	m_pGameInstance.lock()->Update_Engine(fTimeDelta);

	m_pEditorInstance.lock()->Update_Editor(fTimeDelta);

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

HRESULT CMainApp::Ready_Prototype_For_Static_Level()
{
	/* Prototype_GameObject_Camera_Play */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Play"),
		CCamera_Play::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Camera_Play");
		return E_FAIL;
	}

	/* Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Camera_Free");
		return E_FAIL;
	}

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;


	/////////////////////////////////////////////
 /* Prototype_GameObject_EmptyGameObject */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_EmptyGameObject"),
		CEmptyGameObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : EmptyGameObject");
		return E_FAIL;
	}





	///////////////////UItexture/////////////////////


	/* Prototype_Component_Texture_Button */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/Button_Default.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}
	/* Prototype_Component_Texture_Dredge */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Dredge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/Dredge_Title.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}
	/* Prototype_Component_Texture_Select */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Select"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/Selector.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	/////////////////////////TEST//////////////////////////

	//{
	  // lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	   /* Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}









	return S_OK;
}

HRESULT CMainApp::Ready_UI()
{
	////////////////////UI객체원본(프로토 타입X)//////////////////////

	shared_ptr<CUICanvas> pInstance = CUICanvas::Create(m_pDevice, m_pContext);
	pInstance->Initialize(nullptr);


	CUIButton::UIBUTTON_DESC pDesc = {};
	pDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	pDesc.TextureProtoName = L"Prototype_Component_Texture_Button";
	pDesc.OverlapStartEvent = [](CUIButton* pThis) {auto& ch = pThis->GetChildren();
	for (auto& it : ch)
	{
		if (!it) continue;
		it->UI_Active();
		it->m_behavior.push_back((make_shared<CScaleModifier>(0.03f, 4.f, 0.f, 1.8f)));
	}
		};
	pDesc.OverlapEndEvent = [](CUIButton* pThis) {auto& ch = pThis->GetChildren();
	for (auto& it : ch)
	{
		if (!it) continue;
		it->UI_InActive();
		it->m_behavior.clear();
	}
		};
	shared_ptr<CUIButton> pChild = CUIButton::Create(m_pDevice, m_pContext);
	pChild->Initialize(&pDesc);
	//pChild->Set_Zorder(2);
	pInstance->Add_Child(pChild, false);

	CUIImage::UIIMAGE_DESC selectImage{};
	selectImage.TextureComLevel = ETOI(LEVEL::STATIC);
	selectImage.TextureProtoName = L"Prototype_Component_Texture_Select";
	shared_ptr<CUIImage> Select = CUIImage::Create(m_pDevice, m_pContext);
	Select->Initialize(&selectImage);
	Select->UI_InActive();
	pChild->Add_Child(Select, false);

	CUIImage::UIIMAGE_DESC image_desc{};
	image_desc.TextureComLevel = ETOI(LEVEL::STATIC);
	image_desc.TextureProtoName = L"Prototype_Component_Texture_Dredge";
	shared_ptr<CUIImage> pChild2 = CUIImage::Create(m_pDevice, m_pContext);
	pChild2->Initialize(&image_desc);
	// pChild2->Set_Zorder(1);
	pInstance->Add_Child(pChild2, false);

	pChild->Set_Zorder(2);
	pChild2->Set_Zorder(1);

	m_pGameInstance.lock()->UI_InsertToPool(L"MainMenu", pInstance);
	////////


	return S_OK;
}

unique_ptr<CMainApp> CMainApp::Create()
{
	unique_ptr<CMainApp> pInstance(new CMainApp());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
	}
	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	m_pContext->ClearState();

	m_pEditorInstance.lock()->DestroyInstance();
	m_pGameInstance.lock()->DestroyInstance();

	// CData_Manager::GetInstance()->DestroyInstance();

}

