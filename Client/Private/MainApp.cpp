#include "MainApp.h"

#include <UIImage.h>
#include <VIBuffer_Cube.h>

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
#include "DialogueDB.h"
#include "Engine_Struct.h"
#include "Island.h"
#include "ItemDB.h"
#include "Sea.h"
#include "Texture.h"

#include "UI_Controller.h"
#include "Sea_Manager.h"
#include "Sky.h"
#include "VIBuffer_Sea.h"
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
	if (FAILED(Ready_Menu_Prototype_For_Static_Level()))
		return E_FAIL;
	// UI Pool 채우기 / ready_UI 역할
	CUI_Controller::GetInstance()->Initialize(m_pDevice, m_pContext);
	auto SeaManager = CSea_Manager::GetInstance();
	if (SeaManager != nullptr)
	{
		SeaManager->Initialize();
		m_pGameInstance.lock()->Push_ManagerClass(L"Sea_Manager", SeaManager.get());
	}

	if (FAILED((Ready_Fonts())))
		return E_FAIL;

	/* 게임의 시작을 위해 시작이 되는 레벨 할당과 동작을 시킨다 */
	if (FAILED(Ready_StartLevel(LEVEL::LOGO)))
		return E_FAIL;




	//아이템은 아이템 UI 다 프로토 타입 만든후 사용
	CItemDB::GetInstance()->Initialize();
	CDialogueDB::GetInstance()->Ready_DialogueDB();


	return S_OK;
}

int CMainApp::Update(_float fTimeDelta)
{

	m_pGameInstance.lock()->Update_Engine(fTimeDelta);
	CSea_Manager::GetInstance()->Update(fTimeDelta);
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



	m_pEditorInstance.lock()->Render_Editor();


	m_pGameInstance.lock()->Present();

	return S_OK;
}
HRESULT CMainApp::Ready_Fonts()
{
	/*MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:16 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 158ex.spritefont */

	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Hahmlet_SemiBold"), TEXT("../Bin/Resources/Fonts/Hahmlet SemiBold.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Front_Page_Neue"), TEXT("../Bin/Resources/Fonts/Front Page Neue.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Hahmlet_ExtraBold"), TEXT("../Bin/Resources/Fonts/Hahmlet_ExtraBold.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Noto_Sans_CJK_SC"), TEXT("../Bin/Resources/Fonts/Noto Sans CJK SC.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Noto_Sans_CJK_SC_32"), TEXT("../Bin/Resources/Fonts/Noto_Sans_CJK_SC_32.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_Font(TEXT("Noto_Sans_CJK_SC_24"), TEXT("../Bin/Resources/Fonts/Noto_Sans_CJK_SC_24.spritefont"))))
		return E_FAIL;

	/*C:\Users\aes01>C:\MakeSpriteFont.exe "Noto Sans CJK SC" "Noto_Sans_CJK_SC_24.spritefont" /FontSize:24 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD7A3 /DefaultCharacter:0xAC00
Importing Noto Sans CJK SC*/

//MakeSpriteFont.exe "Front Page Neue" / FontSize:16 /FastPack /CharacterRegion : 0x0020 - 0x00FF / CharacterRegion : 0x3131 - 0x3163 / CharacterRegion : 0xAC00 - 0xD800 / DefaultCharacter : 0xAC00 "%USERPROFILE%\Desktop\Font_Page_Neue.spritefont"
/*MakeSpriteFont.exe "Front Page Neue" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Front Page Neue.spritefont"*/
/*MakeSpriteFont.exe "Hahmlet ExtraBold" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Hahmlet_ExtraBold.spritefont"*/
/*MakeSpriteFont.exe "Hahmlet SemiBold" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Hahmlet SemiBold.spritefont"*/
/*MakeSpriteFont.exe "Liberation Sans" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Liberation Sans.spritefont"*/
/*MakeSpriteFont.exe "Noto Sans CJK SC" /FontSize:32 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Noto_Sans_CJK_SC_32.spritefont"*/
/*MakeSpriteFont.exe "Poltawski Nowy" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Poltawski Nowy.spritefont"*/
/*MakeSpriteFont.exe "Perfect DOS VGA 437" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Perfect DOS VGA 437.spritefont"*/
/*MakeSpriteFont.exe "Oswald" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Oswald.spritefont"*/
/*MakeSpriteFont.exe "Signika" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Signika.spritefont"*/
/*MakeSpriteFont.exe "Wingdings" /FontSize:16 /FastPack/CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 "%USERPROFILE%\Desktop\Wingdings.spritefont"*/


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
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxDonut */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxDonut"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxDonut.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxCube");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_Sea */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Sea"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_Sea.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_sea");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxMesh_CustomTexture */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh_CustomTexture"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshCustomTexture.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}

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

	/* Prototype_Component_Texture_EngineEquipmentIcon */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_EngineEquipmentIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/EngineEquipmentIcon_%d.png"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_FishingEquipmentIcon*/
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_FishingEquipmentIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/FishingEquipmentIcon_%d.png"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : CTexture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_LightEquipmentIcon*/
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LightEquipmentIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/LightEquipmentIcon_%d.png"), 2))))
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

	////////tooltip////////

	/* Prototype_Component_Texture_PopupBackground */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_PopupBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inven/PopupBackground.png"), 1))))
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
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/mackerel-ab-1.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/mackerel-ab-1.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/mackerel-ab-2.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/mackerel-ab-2.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/mackerel-ab-3.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/mackerel-ab-3.png"), 1))))
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
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/cod-ab-1.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/cod-ab-1.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/cod-ab-2.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/cod-ab-2.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Fish/cod-ab-3.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Fish/cod-ab-3.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}

	//
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Equip/light1.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Equip/light1.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Equip/engine1.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Equip/engine1.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Equip/rod1.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Equip/rod1.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Mat/lumber.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Mat/lumber.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Mat/cloth.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Mat/cloth.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/Item/Mat/research_item.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Item/Mat/research_item.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
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

	//Prototype_Component_Texture_LoadingIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/EngineIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : LoadingIcon");
		return E_FAIL;
	}


	///////////////////////HUD//////////////////////////////

	//Prototype_Component_Texture_Compass
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Compass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/Compass.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_CompassRing
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_CompassRing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/CompassRing.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}


	//Prototype_Component_Texture_TimeOfDayPointer
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TimeOfDayPointer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/TimeOfDayPointer.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}


	//Prototype_Component_Texture_TimeOfDayWheel
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TimeOfDayWheel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/TimeOfDayWheel.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}


	//Prototype_Component_Texture_HorizontalUITray
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HorizontalUITray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/HorizontalUITray.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_Speaker_Button_Default
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Speaker_Button_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/Speaker_Button_Default.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_ActionButtonMain
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_ActionButtonMain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/ActionButtonMain_%d.png"), 3))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	///////////////////////NPC UI//////////////////////////////

	//Prototype_Component_Texture_TitleBackground
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TitleBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/TitleBackground.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_DialogBox
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_DialogBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/DialogBox_%d.png"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	////Prototype_Component_Texture_Fishmonger
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Fishmonger"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Fishmonger.png"), 1))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Texture");
	//	return E_FAIL;
	//}
	//
	////Prototype_Component_Texture_Fishmonger_Background
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Fishmonger_Background"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Fishmonger_Background.png"), 1))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Texture");
	//	return E_FAIL;
	//}

	//Prototype_Component_Texture_Mayor
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Mayor"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Mayor.png"), 1))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Texture");
	//	return E_FAIL;
	//}
	//
	////Prototype_Component_Texture_Mayor_Background
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Mayor_Background"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Mayor_Background.png"), 1))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Texture");
	//	return E_FAIL;
	//}

	//Prototype_Component_Texture_Shipwright
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/NPC/Shipwright.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Shipwright.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Shipwright_Background
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/NPC/Shipwright_Background.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Shipwright_Background.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	/////////////////////NPC_TEX///////////////////////

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/NPC/Fishmonger_Background.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Fishmonger_Background.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/NPC/Fishmonger.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Fishmonger.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_mackerel");
		return E_FAIL;
	}

	//

	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/NPC/Mayor_Background.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Mayor_Background.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("../Bin/Resources/Textures/NPC/Mayor.png"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/NPC/Mayor.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//

	
	////////////////////////ICON///////////////////////////

	//Prototype_Component_Texture_ShipIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_ShipIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/ShipIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_Texture_LightsIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LightsIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/LightsActionIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_RadialMenuIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_RadialMenuIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/RadialMenuIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_FogHornActionIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_FogHornActionIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HUD/FogHornActionIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	////////////////////////KEY///////////////////////////
	//Prototype_Component_Texture_HorizontalUITray
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_KEY"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Key/keyboard_Icon_%d.png"), 216))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	////////////////////////MINIGAME///////////////////////////
	//Prototype_Component_Texture_MinigamePanel
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MinigamePanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/MinigamePanel.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_Texture_FishingUIBorders
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_FishingUIBorders"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/FishingUIBorders.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_Texture_FishingUICircle
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_FishingUICircle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/FishingUICircle.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_Texture_FishingUISpinner
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_FishingUISpinner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/FishingUISpinner.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_Texture_FishingFishIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_FishingFishIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/FishingFishIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	//Prototype_Component_Texture_CrabPotDepthBar
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_CrabPotDepthBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/CrabPotDepthBar.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;

	}


		//
		//Prototype_Component_Texture_InnerTargetUI
		if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_InnerTargetUI"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/DiamondMinigame_InnerTargetUI.png"), 1))))
		{
			MSG_BOX("Faild to Add_Prototype : Texture");
			return E_FAIL;
		}
		//Prototype_Component_Texture_OuterTargetUI
		if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_OuterTargetUI"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/DiamondMinigame_OuterTargetUI.png"), 1))))
		{
			MSG_BOX("Faild to Add_Prototype : Texture");
			return E_FAIL;
		}
		//Prototype_Component_Texture_DiamondMinigame
		if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_DiamondMinigame"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/DiamondMinigameDiamond.png"), 1))))
		{
			MSG_BOX("Faild to Add_Prototype : Texture");
			return E_FAIL;
		}
		//
			//Prototype_Component_Texture_BallCatcherBall
		if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BallCatcherBall"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/BallCatcherBall_%d.png"), 2))))
		{
			MSG_BOX("Faild to Add_Prototype : Texture");
			return E_FAIL;
		}
		//Prototype_Component_Texture_BallCatcherArrows
		if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BallCatcherArrows"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/BallCatcherArrows.png"), 1))))
		{
			MSG_BOX("Faild to Add_Prototype : Texture");
			return E_FAIL;
		}
		//Prototype_Component_Texture_BallCatcherBG
		if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BallCatcherBG"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MiniGame/BallCatcherBG.png"), 1))))
		{
			MSG_BOX("Faild to Add_Prototype : Texture");
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
	/////////////////////////VILLAGE//////////////////////////
	//Prototype_Component_Texture_VillageUnderBar
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_VillageUnderBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/DestinationPointer.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}


	//Prototype_Component_Texture_DebtBarDectorator
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_DebtBarDectorator"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/DebtBarDectorator.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_SleepIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SleepIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/SleepIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_UndockIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UndockIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/UndockIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_cog_icon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_cog_icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/cog_icon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_MerchantIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MerchantIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/MerchantIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_FishIconVill
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_FishIconVill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/FishIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_StorageIcon
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_StorageIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/StorageIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//shop
	//Prototype_Component_Texture_Shop_Engine
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Shop_Engine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/EngineIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	
	//Prototype_Component_Texture_Shop_Rod
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Shop_Rod"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/RodIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_Shop_Net
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Shop_Net"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/TrawlIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}

	//Prototype_Component_Texture_Shop_Light
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Shop_Light"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Village/LightIcon.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMainApp::Ready_Menu_Prototype_For_Static_Level()
{
	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Cube");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_RGB.png"), 4))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}
	/* Prototype_Component_Texture_Sky_Noise */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/ComplexNoise1_Texture.png"), 4))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Sky_Distortion */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/CloudDistortion_Normal.png"), 4))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}
	/* Prototype_Component_VIBuffer_Sea */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Sea"),
		CVIBuffer_Sea::Create(m_pDevice, m_pContext, 128, 4))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Sea");
		return E_FAIL;
	}

	//Prototype_Component_Texture_TerrainHeight
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TerrainHeight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Terrain_Heightmap_1500.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture");
		return E_FAIL;
	}



	_matrix PreLocalTransformMatrix = { XMMatrixIdentity() };
	/* Prototype_Component_Model_Island */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_Island"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Island/island.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}

	/* Prototype_Component_Model_Island_Side */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_Island_Side"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Island/island_Side.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{ 
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}
	///* Prototype_Component_Model_Rock0 */
	//PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	//if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_Rock0"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Rock/GM_Rock0.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : GM_Town");
	//	return E_FAIL;
	//}
	/* Prototype_Component_Model_Rock0 */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_Rock0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Rock/GM_Rock0.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Rock1 */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_Rock1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Rock/GM_Rock1.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}
	/* Prototype_Component_Model_Rock2 */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_Rock2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Rock/GM_Rock2.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}

	/*Prototype_Component_Model_PineTree*/
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_PineTree"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Tree/Tree_Pine.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}
	/*Prototype_Component_Model_BirchTree*/
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_BirchTree"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Tree/Tree_Birch.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}
	/*Prototype_Component_Model_Bush*/

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f), XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_Bush"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/Tree/Bush.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}



	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_LightHouse"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/BinaryModels/LightHouse/LightHouse_tex.dat", MODEL::NONANIM, PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : GM_Town");
		return E_FAIL;
	}
	///////////////GameObject////////////////////////


	/* Prototype_GameObject_CEmptyGObject */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_CEmptyGObject"),
		CEmptyGObject::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : CEmptyGObject");
		return E_FAIL;
	}



	/* Prototype_GameObject_Island */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Island"),
		CIsland::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Island");
		return E_FAIL;
	}

	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Sky");
		return E_FAIL;
	}

	/* Prototype_GameObject_Sea */
	if (FAILED(m_pGameInstance.lock()->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Sea"),
		CSea::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype :GameObject_Sea");
		return E_FAIL;
	}
}

//HRESULT CMainApp::Ready_UI()
//{
//
//
//	{
//
//		CUIPanel::UIPANEL_DESC LoadingDesc;
//		LoadingDesc.IsFullScreen = true;
//		LoadingDesc.TextureProtoName = L"Prototype_Component_Texture_Black";
//		LoadingDesc.TextureComLevel = ETOI(LEVEL::STATIC);
//		LoadingDesc.IsFullScreen = true;
//		shared_ptr<CUIPanel> pInstance = CUIPanel::Create(m_pDevice, m_pContext);
//		pInstance->Initialize(&LoadingDesc);
//
//		CUIImage::UIIMAGE_DESC LoadingIconDesc;
//		LoadingIconDesc.vPivot = _float2{1.f,0.f};
//		LoadingIconDesc.vAnchorPoint = _float2{ 1.f,0.f };
//		LoadingIconDesc.vAnchoredPos = _float2{ -40.f,40.f };
//		LoadingIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
//		LoadingIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
//		LoadingIconDesc.TextureProtoName = L"Prototype_Component_Texture_LoadingIcon";
//		shared_ptr<CUIImage> pIcon = CUIImage::Create(m_pDevice, m_pContext);
//		pIcon->Initialize(&LoadingIconDesc);
//		pIcon->m_behavior.push_back(make_shared<CRotationModifier>(200.f));
//		pInstance->Add_Child(pIcon, L"ICON_LOADING",false);
//
//
//		CUIText::TEXT_DESC TextDesc;
//		//TextDesc.TextColor = _float4{ 1.f,0.f };
//		TextDesc.strFontTag = L"Noto_Sans_CJK_SC_32";
//		TextDesc.strText = L"심해로부터";
//		
//		shared_ptr<CUIText> text = CUIText::Create(m_pDevice, m_pContext);
//		text->Initialize(&TextDesc);
//		pInstance->Add_Child(text, L"text", false);
//
//
//		m_pGameInstance.lock()->UI_InsertToPool(L"Loading", pInstance);
//
//		
//	}
//
//
//	CUI_MainMenu::MAINMENU_DESC pDescPanel;
//	pDescPanel.IsFullScreen = true;
//	pDescPanel.IsTransparent = true;
//
//	shared_ptr<CUI_MainMenu> pInstance = CUI_MainMenu::Create(m_pDevice, m_pContext);
//	pInstance->Initialize(&pDescPanel);
//	m_pGameInstance.lock()->UI_InsertToPool(L"MainMenu", pInstance);
//	///
//	///
//	///
//	CUI_TabContainer::TABCONTAINER_DESC pDescTap = {};
//	shared_ptr<CUI_TabContainer> TabContainer = CUI_TabContainer::Create(m_pDevice, m_pContext);
//	if (TabContainer == nullptr)
//		return E_FAIL;
//	TabContainer->Initialize(&pDescTap);
//	m_pGameInstance.lock()->UI_InsertToPool(L"TabContainer", TabContainer);
//
//
//
//	CUI_Item::ITEM_DESC pDescitem = {};
//	//CUI_Item::ITEM_DESC pDescitem = {};
//	shared_ptr<CUI_Item>  holdItem =CUI_Item::Create(m_pDevice, m_pContext);
//	if (holdItem == nullptr)
//		return E_FAIL;
//	holdItem->Initialize(&pDescitem);
//	m_pGameInstance.lock()->UI_InsertToPool(L"HoldItem", holdItem);
//
//	////////////HUD///////////////
//	CUI_HUD::HUD_DESC pDescHUD;
//	pDescHUD.IsFullScreen = true;
//	pDescHUD.IsTransparent = true;
//
//	shared_ptr<CUI_HUD> HUD = CUI_HUD::Create(m_pDevice, m_pContext);
//	HUD->Initialize(&pDescHUD);
//	m_pGameInstance.lock()->UI_InsertToPool(L"HUD", HUD);
//
//
//	return S_OK;
//}

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

