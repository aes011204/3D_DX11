#include "UI_MiniGame.h"

#include <UIButton.h>
#include <UIText.h>

#include "ItemDB.h"
#include "UIImage.h"

CUI_MiniGame::CUI_MiniGame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIPanel(pDevice, pContext)
{
}

CUI_MiniGame::CUI_MiniGame(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}
void CUI_MiniGame::UI_PanelActive(MINIGAME MiniGameState, _uint Defid)
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
	Item_Def def = CItemDB::GetInstance()->GetItemByID(Defid);
	m_FishIcon->Change_Texture(def.pTexture);
	m_nameTex->Set_Text(S2W(def.ItemName));
	Set_ActiveForCustom();

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;
	OnActive();

	// 일단 이건 임시
	for (auto& it : m_Children)
	{
		it->UI_Active();
	}
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
	shared_ptr<CUIImage> Border = CUIImage::Create(m_pDevice, m_pContext);
	Border->Initialize(&BorderDesc);

	Add_Child(Border, L"Border", false);

	CUIImage::UIIMAGE_DESC CircleDesc = {};
	CircleDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	CircleDesc.TextureProtoName = L"Prototype_Component_Texture_FishingUICircle";
	CircleDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> Circle = CUIImage::Create(m_pDevice, m_pContext);
	Circle->Initialize(&CircleDesc);
	Circle->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
	Circle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

	Add_Child(Circle, L"Circle", false);
	m_pCircle = Circle;
	m_CircleShader = dynamic_pointer_cast<CShader>(Circle->Get_Component(L"Com_Shader"));

	CUIImage::UIIMAGE_DESC SpinnerDesc = {};
	SpinnerDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	SpinnerDesc.TextureProtoName = L"Prototype_Component_Texture_FishingUISpinner";
	shared_ptr<CUIImage> Spinner = CUIImage::Create(m_pDevice, m_pContext);
	Spinner->Initialize(&SpinnerDesc);

	Add_Child(Spinner, L"Spinner", false);

	CUIImage::UIIMAGE_DESC FishKindIconDesc = {};
	FishKindIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	FishKindIconDesc.TextureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
	shared_ptr<CUIImage> FishKindIcon = CUIImage::Create(m_pDevice, m_pContext);
	FishKindIcon->Initialize(&FishKindIconDesc);
	FishKindIcon->Set_UseColorMix(true);
	FishKindIcon->Set_ColorMix(_float4{ 1.f,1.f,1.f,1.f });

	Circle->Add_Child(FishKindIcon, L"FishKindIcon", false);
	m_FishIcon = FishKindIcon;



	CUIImage::UIIMAGE_DESC DepthBarnDesc = {};
	DepthBarnDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	DepthBarnDesc.TextureProtoName = L"Prototype_Component_Texture_CrabPotDepthBar";
	shared_ptr<CUIImage> DepthBar = CUIImage::Create(m_pDevice, m_pContext);
	DepthBar->Initialize(&DepthBarnDesc);

	Add_Child(DepthBar, L"DepthBar", false);

	CUIImage::UIIMAGE_DESC FishIconDesc = {};
	FishIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	FishIconDesc.TextureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
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
		

	



	///돈///

	{
		
		{
			CUIText::TEXT_DESC nameDesc = {};
			nameDesc.strFontTag = L"Noto_Sans_CJK_SC_32";
			nameDesc.strText = L"$0";
			nameDesc.fontaline = CUIText::FONTALINE::RIGHT;
			shared_ptr<CUIText> nameTex = CUIText::Create(m_pDevice, m_pContext);
			nameTex->Initialize(&nameDesc);
			Add_Child(nameTex, L"nameTex", false);
			m_nameTex = nameTex;

		}
		
		{
			CUIText::TEXT_DESC amountTexDesc = {};
			amountTexDesc.strFontTag = L"Noto_Sans_CJK_SC";
			amountTexDesc.strText = L"amount";
			amountTexDesc.fontaline = CUIText::FONTALINE::RIGHT;
			shared_ptr<CUIText> amountTex = CUIText::Create(m_pDevice, m_pContext);
			amountTex->Initialize(&amountTexDesc);
			Add_Child(amountTex, L"amountTex", false);
			m_amountTex = amountTex;

		}
		{
			CUIText::TEXT_DESC loacationDesc = {};
			loacationDesc.strFontTag = L"Noto_Sans_CJK_SC";
			loacationDesc.strText = L"loaction";
			loacationDesc.fontaline = CUIText::FONTALINE::RIGHT;
			shared_ptr<CUIText> locationTex = CUIText::Create(m_pDevice, m_pContext);
			locationTex->Initialize(&loacationDesc);
			Add_Child(locationTex, L"locationTex", false);
			m_locationTex = locationTex;

		}
	}

	//////////////////////////////////////////////////
	m_zoneCount = 3;
	m_zones[0].start = 0.1f;
	m_zones[0].end = 0.2f;
	m_zones[1].start = 0.3f;
	m_zones[1].end = 0.6f;
	m_zones[2].start = 0.9f;
	m_zones[2].end = .95f;
	return hr;

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
	if(m_CircleShader!=nullptr)
	{
	m_CircleShader->Bind_RawValue("g_zones", m_zones, sizeof(_float4) * 8);
	m_CircleShader->Bind_RawValue("g_zoneCount", &m_zoneCount, sizeof(float));
		
	}


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
