#include "UI_MiniGame.h"


#include <UIButton.h>
#include <UIText.h>

#include "CScaleModifier.h"
#include "ItemDB.h"
#include "UIImage.h"
#include "DInput_Manager.h"
#include "FadeModifier.h"
#include "Inventory_Controller.h"
#include "EventBus.h"
#include "MiniGame_Logic.h"

CUI_MiniGame::CUI_MiniGame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CUIPanel(pDevice, pContext)
{
}

CUI_MiniGame::CUI_MiniGame(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

void CUI_MiniGame::UI_PanelActive()
{
	_uint  m_DefID = m_Logic->Get_DefID();
	if (m_DefID == ID_Absence)
		return;

	m_MiniGameState = m_Logic->Get_MiniGameType();



	//m_DefID 


	////m_Children.clear();
	//_uint numPanel = {};
	//// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	////::OnActive() 에서 클릭한 거만 활성화



	if (m_MiniGameState == MINIGAME::BASIC_CIRCLE)
	{
		m_Spinner->UI_Active();
		m_pCircle->UI_Active();
		m_pCircle->Set_Transparent(false);
	}
	else if (m_MiniGameState == MINIGAME::DIAMOND)
	{
		m_zoneCount = 0;
		m_pSizeCircleIn->UI_Active();
		m_pCircle->UI_Active();
		m_pCircle->Set_Transparent(true);
	}
	else if (m_MiniGameState == MINIGAME::BALL)
	{
		m_pBallCircle->UI_Active();

	}

	Item_Def def = CItemDB::GetInstance()->GetItemByID(m_DefID);

	m_nameTex->UI_Active();
	m_nameTex->Set_Text(S2W(def.ItemName));
	m_amountTex->UI_Active();
	m_locationTex->UI_Active();

	m_pCircleEff->UI_Active();
	m_FishIcon->Change_Texture(def.pTexture);
	m_FishIcon->UI_Active();
	m_Depth->UI_Active();
	//m_PrograssIcon->UI_Active();
	m_Button->UI_Active();
	m_Base->UI_Active();

	Set_ActiveForCustom();

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;
	OnActive();

	// 일단 이건 임시
//	for (auto& it : m_Children)
//	{
//		it->UI_Active();
//	}


}

HRESULT CUI_MiniGame::OnInit(void* pArg)
{




	/*m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_FishingData>(
		[this](const Evt_FishingData& e) {

		}
	);*/


	//m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_MiniGame>(
	//	[this](const Evt_MiniGame& e) {

	//		if(e.IsOnZoon ==true)
	//		{
	//			m_prograssBar01 += 0.2;
	//			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
	//			m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

	//		}
	//		else
	//		{

	//			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
	//			m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
	//			m_pCircle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
	//			m_changeColor = true;
	//			colortime = .5f;
	//		}
	//	}
	//);

	m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_GetFish>(
		[this](const Evt_GetFish& e) {
			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

		}
	);





	//m_Speed = 100.f;
	//m_RodSpeed = .1f;


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
	m_Base = Border;
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


	{
		CUIImage::UIIMAGE_DESC CircleEffDesc = {};
		CircleEffDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		CircleEffDesc.TextureProtoName = L"Prototype_Component_Texture_FishingUICircle";
		shared_ptr<CUIImage> CircleEff = CUIImage::Create(m_pDevice, m_pContext);
		CircleEff->Initialize(&CircleEffDesc);
		//CircleEff->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
		//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

		Circle->Add_Child(CircleEff, L"CircleEff", false);
		m_pCircleEff = CircleEff;
		m_pCircleEff->Set_Alpha(0);
		m_pCircleEff->Set_Zorder(4);
		//m_CircleShader = dynamic_pointer_cast<CShader>(CircleEff->Get_Component(L"Com_Shader"));
	}


	CUIImage::UIIMAGE_DESC SpinnerDesc = {};
	SpinnerDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	SpinnerDesc.TextureProtoName = L"Prototype_Component_Texture_FishingUISpinner";
	shared_ptr<CUIImage> Spinner = CUIImage::Create(m_pDevice, m_pContext);
	Spinner->Initialize(&SpinnerDesc);
	m_Spinner = Spinner;
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
	m_Depth = DepthBar;
	Add_Child(DepthBar, L"DepthBar", false);

	CUIImage::UIIMAGE_DESC FishIconDesc = {};
	FishIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	FishIconDesc.TextureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
	shared_ptr<CUIImage> FishIcon = CUIImage::Create(m_pDevice, m_pContext);
	FishIcon->Initialize(&FishIconDesc);

	DepthBar->Add_Child(FishIcon, L"FishIcon", false);
	m_PrograssIcon = FishIcon;



	/// 버튼
	CUIButton::UIBUTTON_DESC ButDesc = {};
	ButDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	ButDesc.TextureProtoName = L"Prototype_Component_Texture_Button_RED";
	ButDesc.bUseDark =true;

	ButDesc.OverlapStartEvent = [](CUIButton* pThis) {};
	ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
	ButDesc.ClickEvent = [this](CUIButton* pThis)
		{
			if (m_Logic)
				m_Logic->OnInput();
		};
	shared_ptr<CUIButton> button = CUIButton::Create(m_pDevice, m_pContext);
	button->Initialize(&ButDesc);

	button->Set_SelectState(false);
	m_Button = button;
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
	//////////////////////////////////////////////////////////

	CUIImage::UIIMAGE_DESC SizeCircleInDesc = {};
	SizeCircleInDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	SizeCircleInDesc.TextureProtoName = L"Prototype_Component_Texture_InnerTargetUI";
	shared_ptr<CUIImage> SizeCircleIn = CUIImage::Create(m_pDevice, m_pContext);
	SizeCircleIn->Initialize(&SizeCircleInDesc);


	Add_Child(SizeCircleIn, L"SizeCircleIn", false);
	m_pSizeCircleIn = SizeCircleIn;

	{
		CUIImage::UIIMAGE_DESC SizeCircleOutDesc = {};
		SizeCircleOutDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		SizeCircleOutDesc.TextureProtoName = L"Prototype_Component_Texture_OuterTargetUI";
		shared_ptr<CUIImage> SizeCircleOut = CUIImage::Create(m_pDevice, m_pContext);
		SizeCircleOut->Initialize(&SizeCircleOutDesc);


		SizeCircleIn->Add_Child(SizeCircleOut, L"SizeCircleOut", false);
		//m_pSizeCircleOut = SizeCircleIn;

		{

			CUIImage::UIIMAGE_DESC SizeCircleDesc = {};
			SizeCircleDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			SizeCircleDesc.TextureProtoName = L"Prototype_Component_Texture_DiamondMinigame";
			shared_ptr<CUIImage> SizeCircle = CUIImage::Create(m_pDevice, m_pContext);
			SizeCircle->Initialize(&SizeCircleDesc);

			SizeCircle->Set_UseColorMix(true);
			SizeCircle->Set_ColorMix(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });
			//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			SizeCircleOut->Add_Child(SizeCircle, L"SizeCircleEff", false);
			m_pSizeCircle = SizeCircle;
			m_pCircleEff->Set_Zorder(4);
			//m_CircleShader = dynamic_pointer_cast<CShader>(CircleEff->Get_Component(L"Com_Shader"));

		}
	}

	///////////////////////////////////

	CUIImage::UIIMAGE_DESC BallCircleInDesc = {};
	BallCircleInDesc.TextureComLevel = ETOI(LEVEL::STATIC);
	BallCircleInDesc.TextureProtoName = L"Prototype_Component_Texture_InnerTargetUI";
	BallCircleInDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> BallCircleIn = CUIImage::Create(m_pDevice, m_pContext);
	BallCircleIn->Initialize(&BallCircleInDesc);


	Add_Child(BallCircleIn, L"BallCircleIn", false);
	m_pBallCircle = BallCircleIn;

	{
		CUIImage::UIIMAGE_DESC BallStartDesc = {};
		BallStartDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		BallStartDesc.TextureProtoName = L"Prototype_Component_Texture_OuterTargetUI";
		BallStartDesc.shaderType = CUIRenderable::UIShaderType::Radial;
		shared_ptr<CUIImage> BallStart = CUIImage::Create(m_pDevice, m_pContext);
		BallStart->Initialize(&BallStartDesc);


		BallCircleIn->Add_Child(BallStart, L"BallStart", false);
		//m_pSizeCircleOut = SizeCircleIn;




		CUIImage::UIIMAGE_DESC BallDesc = {};
		BallDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		BallDesc.TextureProtoName = L"Prototype_Component_Texture_DiamondMinigame";
		shared_ptr<CUIImage> Ball = CUIImage::Create(m_pDevice, m_pContext);
		Ball->Initialize(&BallDesc);
		
		//CircleEff->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
		//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

		BallCircleIn->Add_Child(Ball, L"Ball", false);
		m_pBall = Ball;

		//m_CircleShader = dynamic_pointer_cast<CShader>(CircleEff->Get_Component(L"Com_Shader"));

	}

	//////////////////////TEST//////////////////////////////
	//m_zoneCount = 3;
	//m_zones[0].start = 0.1f;
	//m_zones[0].end = 0.2f;
	//m_zones[1].start = 0.3f;
	//m_zones[1].end = 0.6f;
	//m_zones[2].start = 0.9f;
	//m_zones[2].end = .95f;

	//m_FishCount = 4;

	return hr;

}

void CUI_MiniGame::Is_OnZoon(_bool IsOnZoon)
{
	if (IsOnZoon == true)
	{
		m_prograssBar01 += 0.2;
		m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
		m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

	}
	else
	{

		m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
		m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
		m_pCircle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
		m_changeColor = true;
		colortime = .5f;
	}
}


void CUI_MiniGame::OnActive()
{

	m_Spinner->GetUITransform()->SetRotation(0);
	m_prograssBar01 = 0;
	m_bFin = false;



	CUIPanel::OnActive();
}

void CUI_MiniGame::OnInActive()
{
	m_DefID = ID_Absence;
	CUIPanel::OnInActive();
}

void CUI_MiniGame::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_MiniGame::OnUpdate(const _float& timeDelta)
{


	if (!m_Logic)
		return;


	switch(m_MiniGameState)
	{
	case MINIGAME::BASIC_CIRCLE:
		{
		m_zoneCount = m_Logic->GetZoneCount();
		memcpy(m_zones, m_Logic->GetZones(), sizeof(Zone) * m_zoneCount);

		

		auto result = m_Logic->ConsumeInputResult();
		if (result == INPUT_RESULT::SUCCESS)
		{
			// 초록 효과
			m_prograssBar01 += 0.2;

			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));
		
		}
		else if (result == INPUT_RESULT::FAIL)
		{
			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
			m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
			m_pCircle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
			m_changeColor = true;
			colortime = .5f;
			// 빨강 효과
		}

		m_Spinner->GetUITransform()->SetRotation(-(m_Logic->GetAngle()));

		
		if (m_changeColor == true)
		{
			m_AccTime2 += timeDelta;
			if (m_AccTime2 >= colortime)
			{
				m_AccTime2 = 0;
				m_changeColor = false;
				m_pCircle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			}
		}

		}

		break;
	case MINIGAME::DIAMOND:
		{
	

			auto result = m_Logic->ConsumeInputResult();

		
			m_pSizeCircle->GetUITransform()->SetLocalScale(_float2{ m_Logic->GetSize(), m_Logic->GetSize() });
		m_pSizeCircle->GetUITransform()->SetRotation(m_Logic->GetAngle());
		
			/*if(m_Logic->GetSize() > 1.f)
			{
				m_pSizeCircle->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .5f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));

			}*/
			if(result == INPUT_RESULT::START)
			{
				m_pSizeCircle->UI_Active();
;				//m_pSizeCircle->GetUITransform()->SetLocalScale(_float2{ 0.f,0.f });
				m_pSizeCircle->m_behavior.clear();
				m_pSizeCircle->Set_Alpha(1.f);

				m_pSizeCircle->Set_ColorMix(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });
			}

		if (result == INPUT_RESULT::SUCCESS)
		{
			// 초록 효과
			m_prograssBar01 += 0.2;
			m_pSizeCircle->m_behavior.clear();
			m_pSizeCircle->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .5f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, true));
			m_pSizeCircle->Set_ColorMix(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			//m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));
			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT,1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			LOG(LOG_LEVEL::INFO, "SUCCES");
		}
		else if (result == INPUT_RESULT::FAIL)
		{
			m_pSizeCircle->m_behavior.clear();
			m_pSizeCircle->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT,.5f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, true));
			m_pSizeCircle->Set_ColorMix(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
			//
			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
			m_changeColor = true;
			colortime = .5f;
			// 빨강 효과
		}

		if (m_changeColor == true)
		{
			m_AccTime2 += timeDelta;
			if (m_AccTime2 >= colortime)
			{
				m_AccTime2 = 0;
				m_changeColor = false;
				//m_pCircle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			}
		}
		
		}

		break;
	case MINIGAME::BALL:

		auto result = m_Logic->ConsumeInputResult();
		if (result == INPUT_RESULT::SUCCESS)
		{
			// 초록 효과
			m_prograssBar01 += 0.2;

			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

		}
		else if (result == INPUT_RESULT::FAIL)
		{
			m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
			m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
			m_pCircle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
			m_changeColor = true;
			colortime = .5f;
			// 빨강 효과
		}
		break;
	}

	float progress = m_Logic->GetProgress01();
	m_PrograssIcon->GetUITransform()->SetAnchoredPos(
		_float2{
			m_PrograssIcon->GetUITransform()->Get_AnchoredPos().x,
			lerp(0.f, 239.f, progress)
		}
	);

	m_amountTex->Set_Text(format(L"{}", m_Logic->GetFishCount()));
	


	CUIPanel::OnUpdate(timeDelta);
}

void CUI_MiniGame::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_MiniGame::OnRender()
{
	if (m_CircleShader != nullptr)
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
