#include "UI_MiniGame.h"

#include <UIButton.h>
#include <UIText.h>

#include "UIImage.h"

CUI_MiniGame::CUI_MiniGame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIPanel(pDevice, pContext)
{
}

CUI_MiniGame::CUI_MiniGame(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}
void CUI_MiniGame::UI_PanelActive(MINIGAME MiniGameState)
{
	////m_Children.clear();
	//_uint numPanel = {};
	//// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	////::OnActive() 에서 클릭한 거만 활성화



	//if (MiniGameState == MINIGAME::BASIC_CIRCLE)
	//{
	//	//Add_Child(m_ButtonContents[ETOI(TAB::INVEN)], L"BUTTON_INVEN", false);
	//	(m_ButtonContents[ETOI(TAB::INVEN)]->UI_Active());
	//	//Add_Child(m_TabContents[ETOI(TAB::INVEN)], L"INVEN", false);
	//	m_TabContents[ETOI(TAB::INVEN)]->UI_InActive();
	//	numPanel++;
	//}
	//if (iTabfig & ETOI(TAB::STORAGE))
	//{
	//	//(m_ButtonContents[ETOI(TAB::STORAGE)], L"BUTTON_STORAGE", false);
	//	(m_ButtonContents[ETOI(TAB::STORAGE)]->UI_Active());
	//	//Add_Child(m_TabContents[ETOI(TAB::STORAGE)], L"STORAGE", false);
	//	m_TabContents[ETOI(TAB::STORAGE)]->UI_InActive();
	//	numPanel++;

	//}
	//if (iTabfig & ETOI(TAB::ETC))
	//{
	//	//(m_ButtonContents[ETOI(TAB::ETC)], L"BUTTON_ETC", false);
	//	m_ButtonContents[ETOI(TAB::ETC)]->UI_Active();
	//	//Add_Child(m_TabContents[ETOI(TAB::ETC)], L"ETC", false);
	//	m_TabContents[ETOI(TAB::ETC)]->UI_InActive();
	//	numPanel++;
	//}
	//m_Line->UI_Active();
	//m_TextIMG->UI_Active();

	//Change_LayoutRawCol(numPanel, 1);

	//m_Active = Active;

	//Set_ActiveForCustom();

	//m_bRenderReady = false;

	//if (m_pUITransformCom)
	//	m_pUITransformCom->UpdateLayoutIfDirty();

	////m_bInteractable = true;
	//OnActive();


}

HRESULT CUI_MiniGame::OnInit(void* pArg)
{
	//m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_AddMoney>([this](const Evt_AddMoney& e)
	//	{

	//		wstring strMoney = format(L"${:.2f}", e.money);
	//		//wstring strM = L"$";
	//		this->m_TextFont->Set_Text(/*strM +*/ strMoney);
	//	});



	HRESULT hr = E_FAIL;
	MINIGAEMEPANEL_DESC* PanelDesc = static_cast<MINIGAEMEPANEL_DESC*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	PanelDesc->TextureComLevel = ETOI(LEVEL::STATIC);
	PanelDesc->TextureProtoName = L"Prototype_Component_Texture_MinigamePanel";

	

	hr = CUIPanel::OnInit(PanelDesc);


	CUIImage::UIIMAGE_DESC BorderDesc = {};
	BorderDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	BorderDesc.TextureProtoName = L"Prototype_Component_Texture_FishingUIBorders";
	BorderDesc.bUseNineSlice = true;
	shared_ptr<CUIImage> Border = CUIImage::Create(m_pDevice, m_pContext);
	Border->Initialize(&BorderDesc);

	Add_Child(Border, L"Border", false);

	CUIImage::UIIMAGE_DESC CircleDesc = {};
	CircleDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	CircleDesc.TextureProtoName = L"Prototype_Component_Texture_FishingUICircle";
	CircleDesc.bUseNineSlice = true;
	shared_ptr<CUIImage> Circle = CUIImage::Create(m_pDevice, m_pContext);
	Circle->Initialize(&CircleDesc);

	Add_Child(Circle, L"Circle", false);

	CUIImage::UIIMAGE_DESC SpinnerDesc = {};
	SpinnerDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	SpinnerDesc.TextureProtoName = L"Prototype_Component_Texture_FishingUISpinner";
	SpinnerDesc.bUseNineSlice = true;
	shared_ptr<CUIImage> Spinner = CUIImage::Create(m_pDevice, m_pContext);
	Spinner->Initialize(&SpinnerDesc);

	Add_Child(Spinner, L"Spinner", false);






	CUIImage::UIIMAGE_DESC DepthBarnDesc = {};
	DepthBarnDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	DepthBarnDesc.TextureProtoName = L"Prototype_Component_Texture_CrabPotDepthBar";
	DepthBarnDesc.bUseNineSlice = true;
	shared_ptr<CUIImage> DepthBar = CUIImage::Create(m_pDevice, m_pContext);
	DepthBar->Initialize(&DepthBarnDesc);

	Add_Child(DepthBar, L"DepthBar", false);

	CUIImage::UIIMAGE_DESC FishIconDesc = {};
	FishIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	FishIconDesc.TextureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
	FishIconDesc.bUseNineSlice = true;
	shared_ptr<CUIImage> FishIcon = CUIImage::Create(m_pDevice, m_pContext);
	FishIcon->Initialize(&FishIconDesc);

	DepthBar->Add_Child(FishIcon, L"FishIcon", false);

		/// 버튼
		CUIButton::UIBUTTON_DESC ButDesc = {};
		ButDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		ButDesc.TextureProtoName = L"Prototype_Component_Texture_Button_RED";
		ButDesc.OverlapStartEvent = [](CUIButton* pThis) {};
		ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc.ClickEvent = [this](CUIButton* pThis)
			{
				int i = 1;
			};
		shared_ptr<CUIButton> button = CUIButton::Create(m_pDevice, m_pContext);
		button->Initialize(&ButDesc);

		{

			CUIText::TEXT_DESC text_Desc = {};
			text_Desc.strFontTag = L"Noto_Sans_CJK_SC_24";
			text_Desc.strText = L"당기기";
			shared_ptr<CUIText> Text_storage = CUIText::Create(m_pDevice, m_pContext);
			Text_storage->Initialize(&text_Desc);
			button->Add_Child(Text_storage, L"Text_Pull", false);
		}

		//Add_Child(pChild, NameTag, false);
		wstring NameTag = L"BUTTON";
		Add_Layout_Child(button, NameTag, false);
		

	

	
		//TAB eTab = static_cast<TAB>(1 << 0);
		//// 일단 테스트 인벤 3개
		//CUI_Inventory::INVENTORY_DESC InvenDesc = {};
		//InvenDesc.IsFullScreen = false;
		//InvenDesc.IsTransparent = true;
		//InvenDesc.bSetParentSize = true;

		//shared_ptr<CUI_Inventory> pInven = CUI_Inventory::Create(m_pDevice, m_pContext);
		//pInven->Initialize(&InvenDesc);

		//wstring NameTag = S2W(string(magic_enum::enum_name(eTab)));
		//Add_Child(pInven, NameTag, false);
		//m_TabContents[ETOI(eTab)] = pInven;
		//pInven->UI_InActive();
	
		//TAB eTab = static_cast<TAB>(1 << 1);
		//// 일단 테스트 인벤 3개
		//CUI_Storage::STORAGE_DESC StorageDesc = {};
		//StorageDesc.IsFullScreen = false;
		//StorageDesc.IsTransparent = true;
		//StorageDesc.bSetParentSize = true;

		//shared_ptr<CUI_Storage> pStorage = CUI_Storage::Create(m_pDevice, m_pContext);
		//pStorage->Initialize(&StorageDesc);




		//wstring NameTag = S2W(string(magic_enum::enum_name(eTab)));
		//Add_Child(pStorage, NameTag, false);
		//m_TabContents[ETOI(eTab)] = pStorage;
		//pStorage->UI_InActive();


	///돈///

	{
		
		{
			CUIText::TEXT_DESC MoneyTexDesc = {};
			MoneyTexDesc.strFontTag = L"Noto_Sans_CJK_SC";
			MoneyTexDesc.strText = L"$0";
			MoneyTexDesc.fontaline = CUIText::FONTALINE::RIGHT;
			shared_ptr<CUIText> MoneyTex = CUIText::Create(m_pDevice, m_pContext);
			MoneyTex->Initialize(&MoneyTexDesc);
			Add_Child(MoneyTex, L"MoneyTex", false);
			
		}
		
		
	}

	return hr;

	return CUIPanel::OnInit(pArg);
}

void CUI_MiniGame::OnActive()
{
	CUIPanel::OnActive();
}

void CUI_MiniGame::OnInActive()
{
	CUIPanel::OnInActive();
}

void CUI_MiniGame::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_MiniGame::OnUpdate(const _float& timeDelta)
{
	CUIPanel::OnUpdate(timeDelta);
}

void CUI_MiniGame::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_MiniGame::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_MiniGame::OnClear()
{
	CUIPanel::OnClear();
}

shared_ptr<CUI_MiniGame> CUI_MiniGame::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_MiniGame> pInstance(new CUI_MiniGame(pDevice, pContext), [](CUI_MiniGame* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_MiniGame::Free()
{
	CUIPanel::Free();
}
