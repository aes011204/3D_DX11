#include "MainApp.h"

#include <UIImage.h>

#include "GameInstance.h"
#include "Client_Define.h"
#include "Data_Manager.h"
#include "Inventory.h"
#include "Level_Loading.h"
#include "EditorInstance.h"
#include "EmptyUObject.h"
#include "EmptyGObject.h"
#include "Camera_Play.h"
#include "Camera_Free.h"
#include "Engine_Struct.h"
#include "Inventory_Controller.h"
#include "ItemDB.h"
#include "Texture.h"

#include "UI_MainMenu.h"
#include "UI_TabContainer.h"
#include "UI_Item.h"

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

	if (FAILED(Ready_Prototype_For_Static_Level()))
		return E_FAIL;


	if (FAILED((Ready_UI())))
		return E_FAIL;

	if (FAILED((Ready_Fonts())))
		return E_FAIL;

	/* 게임의 시작을 위해 시작이 되는 레벨 할당과 동작을 시킨다 */
	if (FAILED(Ready_StartLevel(LEVEL::LOGO)))
		return E_FAIL;



  
	//아이템은 아이템 UI 다 프로토 타입 만든후 사용
	CItemDB ::GetInstance()->Initialize();
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

	m_pGameInstance.lock()->Bind_BackBufferRenderTarget(g_hWnd);

	m_pGameInstance.lock()->Draw();

	m_pGameInstance.lock()->Font_Draw(TEXT("Hahmlet_SemiBold"), TEXT("니네들은 싸우지마"), _float2(100.f, 0.f));



	m_pEditorInstance.lock()->Render_Editor();


	m_pGameInstance.lock()->Present();

	return S_OK;
}
HRESULT CMainApp::Ready_Fonts()
{
	/*MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:16 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 158ex.spritefont */

	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Hahmlet_SemiBold"), TEXT("../Bin/Resources/Fonts/Hahmlet SemiBold.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Front_Page_Neue."), TEXT("../Bin/Resources/Fonts/Front Page Neue.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Hahmlet_ExtraBold"), TEXT("../Bin/Resources/Fonts/Hahmlet_ExtraBold.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Liberation_Sans"), TEXT("../Bin/Resources/Fonts/Liberation Sans.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Noto_Sans_CJK_SC"), TEXT("../Bin/Resources/Fonts/Noto Sans CJK SC.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Poltawski_Nowy"), TEXT("../Bin/Resources/Fonts/Poltawski Nowy.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Perfect_DOS_VGA_437"), TEXT("../Bin/Resources/Fonts/Perfect DOS VGA 437.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Oswald"), TEXT("../Bin/Resources/Fonts/Oswald.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Signika"), TEXT("../Bin/Resources/Fonts/Signika.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Wingdings"), TEXT("../Bin/Resources/Fonts/Wingdings.spritefont"))))
		return E_FAIL;

	//MakeSpriteFont.exe "Front Page Neue" / FontSize:16 /CharacterRegion : 0x0020 - 0x00FF / CharacterRegion : 0x3131 - 0x3163 / CharacterRegion : 0xAC00 - 0xD800 / DefaultCharacter : 0xAC00 "%USERPROFILE%\Desktop\Font_Page_Neue.spritefont"
	/*MakeSpriteFont.exe "Front Page Neue" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Front Page Neue.spritefont"*/
	/*MakeSpriteFont.exe "Hahmlet ExtraBold" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Hahmlet_ExtraBold.spritefont"*/
	/*MakeSpriteFont.exe "Hahmlet SemiBold" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Hahmlet SemiBold.spritefont"*/
	/*MakeSpriteFont.exe "Liberation Sans" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Liberation Sans.spritefont"*/
	/*MakeSpriteFont.exe "Noto Sans CJK SC" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Noto Sans CJK SC.spritefont"*/
	/*MakeSpriteFont.exe "Poltawski Nowy" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Poltawski Nowy.spritefont"*/
	/*MakeSpriteFont.exe "Perfect DOS VGA 437" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Perfect DOS VGA 437.spritefont"*/
	/*MakeSpriteFont.exe "Oswald" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Oswald.spritefont"*/
	/*MakeSpriteFont.exe "Signika" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Signika.spritefont"*/
	/*MakeSpriteFont.exe "Wingdings" /FontSize:16 /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Wingdings.spritefont"*/


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
	////////////////////////CAMERA////////////////////////

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



	////////////////////////SHADER////////////////////////
	///
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	////////////////////////COMPONENT////////////////////////

	/* Prototype_Component_Inven */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Inven"),
		CInventory::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Component_Inven ");
		return E_FAIL;
	}

	///////////////////////////////////////////////////////

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/////////////////////////////////////////////
 /* Prototype_GameObject_CEmptyUObject */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_CEmptyUObject"),
		CEmptyUObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : CEmptyUObject");
		return E_FAIL;
	}


	/* Prototype_GameObject_CEmptyGObject */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_CEmptyGObject"),
		CEmptyGObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : CEmptyGObject");
		return E_FAIL;
	}


	///////////////////UItexture/////////////////////

	// 메인메뉴 //
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


	// 인밴 //
	/* Prototype_Component_Texture_Button_RED */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Button_RED"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/Tab_Selected.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Slot_Damage */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Slot_Damage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/DamageBox_%d.png"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Slot_Inven */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Slot_Inven"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/CargoGrid_%d.png"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_HealthBarPanel */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HealthBarPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/HealthBarPanel.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}
	/* Prototype_Component_Texture_PanelBackground */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_PanelBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/PanelBackground.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_PlayerInventoryBackground */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_PlayerInventoryBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/PlayerInventoryBackground.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	// 창고 //
	/* Prototype_Component_Texture_StorageInventoryBackground */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_StorageInventoryBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Panel/StorageInventoryBackground.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}
	/* Prototype_Component_Texture_TabDivider */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TabDivider"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Panel/TabDivider.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	// tab_Container //
	/* Prototype_Component_Texture_TabContainer */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TabContainer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Panel/SidePanel.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_TabContainer");
		return E_FAIL;
	}



	/////////////////////ITEM_TEX///////////////////////

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/mackerel.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/mackerel.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/cod.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/cod.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}


	///////////////////////LOADING//////////////////////////////

	//Prototype_Component_Texture_Black
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Black.jpg"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Black");
		return E_FAIL;
	}



	/////////////////////////TEST//////////////////////////

	//{
	  // lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	   /* Prototype_Component_Texture_BackGround */
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : CTexture");
	//	return E_FAIL;
	//}


	





	return S_OK;
}

HRESULT CMainApp::Ready_UI()
{


	{
		CUIPanel::UIPANEL_DESC LoadingDesc;
		LoadingDesc.IsFullScreen = true;
		LoadingDesc.TextureProtoName = L"Prototype_Component_Texture_Black";
		LoadingDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		shared_ptr<CUIPanel> pInstance = CUIPanel::Create(m_pDevice, m_pContext);
		pInstance->Initialize(&LoadingDesc);
		m_pGameInstance.lock()->UI_InsertToPool(L"Loading", pInstance);
	}


	CUI_MainMenu::MAINMENU_DESC pDescPanel;
	pDescPanel.IsFullScreen = true;
	pDescPanel.IsTransparent = true;

	shared_ptr<CUI_MainMenu> pInstance = CUI_MainMenu::Create(m_pDevice, m_pContext);
	pInstance->Initialize(&pDescPanel);
	m_pGameInstance.lock()->UI_InsertToPool(L"MainMenu", pInstance);
	///
	///
	///
	CUI_TabContainer::TABCONTAINER_DESC pDescTap = {};
	shared_ptr<CUI_TabContainer> TabContainer = CUI_TabContainer::Create(m_pDevice, m_pContext);
	if (TabContainer == nullptr)
		return E_FAIL;
	TabContainer->Initialize(&pDescTap);
	m_pGameInstance.lock()->UI_InsertToPool(L"TabContainer", TabContainer);



	CUI_Item::ITEM_DESC pDescitem = {};
	//CUI_Item::ITEM_DESC pDescitem = {};
	shared_ptr<CUI_Item>  holdItem =CUI_Item::Create(m_pDevice, m_pContext);
	if (holdItem == nullptr)
		return E_FAIL;
	holdItem->Initialize(&pDescitem);
	m_pGameInstance.lock()->UI_InsertToPool(L"HoldItem", holdItem);

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

	//CInventory_Controller::DestroyInstance();
	CItemDB::DestroyInstance();

	m_pContext->ClearState();

	m_pEditorInstance.lock()->DestroyInstance();
	m_pGameInstance.lock()->DestroyInstance();

	// CData_Manager::GetInstance()->DestroyInstance();

}

