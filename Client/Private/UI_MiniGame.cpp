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
	_uint  m_defId = m_logic->Get_DefID();
	if (m_defId == kIdAbsence)
		return;

	m_miniGameState = m_logic->Get_MiniGameType();



	//m_DefID 


	////m_Children.clear();
	//_uint numPanel = {};
	//// 일단 기본적으로 패널 은 다 inactive, 버튼은 iTabfig 에 따라 active
	////::OnActive() 에서 클릭한 거만 활성화



	if (m_miniGameState == EMiniGameType::BASIC_CIRCLE)
	{
		m_spinner->UI_Active();
		m_circle->UI_Active();
		m_circle->Set_Transparent(false);
	}
	else if (m_miniGameState == EMiniGameType::DIAMOND)
	{
		m_zoneCount = 0;
		m_sizeCircleIn->UI_Active();
		m_circle->UI_Active();
		m_circle->Set_Transparent(true);
	}
	else if (m_miniGameState == EMiniGameType::BALL)
	{
		m_ballCircle->UI_Active();

	}

	ItemDef def = CItemDB::GetInstance()->GetItemByID(m_defId);

	m_nameTex->UI_Active();
	m_nameTex->Set_Text(S2W(def.itemName));
	m_amountTex->UI_Active();
	m_locationTex->UI_Active();

	m_circleEff->UI_Active();
	m_fishIcon->Change_Texture(def.texture);
	m_fishIcon->UI_Active();
	m_depth->UI_Active();
	//m_PrograssIcon->UI_Active();
	m_button->UI_Active();
	m_base->UI_Active();

	Set_ActiveForCustom();

	m_isRenderReady = false;

	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();

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

	m_gameInstance.lock()->Get_EventBus()->Subscribe<Evt_GetFish>(
		[this](const Evt_GetFish& e) {
			m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			m_circleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

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
	MiniGamePanelDesc* PanelDesc = static_cast<MiniGamePanelDesc*>(pArg);
	//TABCONTAINER_DESC pDesc = {};
	PanelDesc->textureComLevel = ETOI(ELevel::STATIC);
	PanelDesc->textureProtoName = L"Prototype_Component_Texture_MinigamePanel";



	hr = CUIPanel::OnInit(PanelDesc);


	CUIImage::UIImageDesc BorderDesc = {};
	BorderDesc.textureComLevel = ETOI(ELevel::STATIC);
	BorderDesc.textureProtoName = L"Prototype_Component_Texture_FishingUIBorders";
	shared_ptr<CUIImage> Border = CUIImage::Create(m_device, m_context);
	Border->Initialize(&BorderDesc);
	m_base = Border;
	Add_Child(Border, L"Border", false);

	CUIImage::UIImageDesc CircleDesc = {};
	CircleDesc.textureComLevel = ETOI(ELevel::STATIC);
	CircleDesc.textureProtoName = L"Prototype_Component_Texture_FishingUICircle";
	CircleDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> Circle = CUIImage::Create(m_device, m_context);
	Circle->Initialize(&CircleDesc);
	Circle->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
	Circle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

	Add_Child(Circle, L"Circle", false);
	m_circle = Circle;
	m_circleShader = dynamic_pointer_cast<CShader>(Circle->Get_Component(L"Com_Shader"));


	{
		CUIImage::UIImageDesc CircleEffDesc = {};
		CircleEffDesc.textureComLevel = ETOI(ELevel::STATIC);
		CircleEffDesc.textureProtoName = L"Prototype_Component_Texture_FishingUICircle";
		shared_ptr<CUIImage> CircleEff = CUIImage::Create(m_device, m_context);
		CircleEff->Initialize(&CircleEffDesc);
		//CircleEff->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
		//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

		Circle->Add_Child(CircleEff, L"CircleEff", false);
		m_circleEff = CircleEff;
		m_circleEff->Set_Alpha(0);
		m_circleEff->Set_Zorder(4);
		//m_CircleShader = dynamic_pointer_cast<CShader>(CircleEff->Get_Component(L"Com_Shader"));
	}


	CUIImage::UIImageDesc SpinnerDesc = {};
	SpinnerDesc.textureComLevel = ETOI(ELevel::STATIC);
	SpinnerDesc.textureProtoName = L"Prototype_Component_Texture_FishingUISpinner";
	shared_ptr<CUIImage> Spinner = CUIImage::Create(m_device, m_context);
	Spinner->Initialize(&SpinnerDesc);
	m_spinner = Spinner;
	Add_Child(Spinner, L"Spinner", false);

	CUIImage::UIImageDesc FishKindIconDesc = {};
	FishKindIconDesc.textureComLevel = ETOI(ELevel::STATIC);
	FishKindIconDesc.textureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
	shared_ptr<CUIImage> FishKindIcon = CUIImage::Create(m_device, m_context);
	FishKindIcon->Initialize(&FishKindIconDesc);
	FishKindIcon->Set_UseColorMix(true);
	FishKindIcon->Set_ColorMix(_float4{ 1.f,1.f,1.f,1.f });

	Circle->Add_Child(FishKindIcon, L"FishKindIcon", false);
	m_fishIcon = FishKindIcon;



	CUIImage::UIImageDesc DepthBarnDesc = {};
	DepthBarnDesc.textureComLevel = ETOI(ELevel::STATIC);
	DepthBarnDesc.textureProtoName = L"Prototype_Component_Texture_CrabPotDepthBar";
	shared_ptr<CUIImage> DepthBar = CUIImage::Create(m_device, m_context);
	DepthBar->Initialize(&DepthBarnDesc);
	m_depth = DepthBar;
	Add_Child(DepthBar, L"DepthBar", false);

	CUIImage::UIImageDesc FishIconDesc = {};
	FishIconDesc.textureComLevel = ETOI(ELevel::STATIC);
	FishIconDesc.textureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
	shared_ptr<CUIImage> FishIcon = CUIImage::Create(m_device, m_context);
	FishIcon->Initialize(&FishIconDesc);

	DepthBar->Add_Child(FishIcon, L"FishIcon", false);
	m_progressIcon = FishIcon;



	/// 버튼
	CUIButton::UIButtonDesc ButDesc = {};
	ButDesc.textureComLevel = ETOI(ELevel::STATIC);
	ButDesc.textureProtoName = L"Prototype_Component_Texture_Button_RED";
	ButDesc.useDark =true;

	ButDesc.OverlapStartEvent = [this](CUIButton* pThis)
	{
			m_gameInstance.lock()->Play_Once(L"Hover");

	};
	ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
	ButDesc.ClickEvent = [this](CUIButton* pThis)
		{
			if (m_logic)
				m_logic->OnInput();

			//m_pGameInstance.lock()->Play_Loop(L"fishing_loop");
			
		};
	shared_ptr<CUIButton> button = CUIButton::Create(m_device, m_context);
	button->Initialize(&ButDesc);

	button->Set_SelectState(false);
	m_button = button;
	{
		CUIText::TextDesc text_Desc = {};
		text_Desc.fontTag = L"Noto_Sans_CJK_SC_24";
		text_Desc.text = L"당기기";
		shared_ptr<CUIText> Text_storage = CUIText::Create(m_device, m_context);
		Text_storage->Initialize(&text_Desc);
		button->Add_Child(Text_storage, L"Text_Pull", false);
	}

	//Add_Child(pChild, NameTag, false);
	wstring NameTag = L"BUTTON";
	Add_Layout_Child(button, NameTag, false);






	///돈///

	{

		{
			CUIText::TextDesc nameDesc = {};
			nameDesc.fontTag = L"Noto_Sans_CJK_SC_32";
			nameDesc.text = L"$0";
			nameDesc.fontAlign = CUIText::EFontAlign::RIGHT;
			shared_ptr<CUIText> nameTex = CUIText::Create(m_device, m_context);
			nameTex->Initialize(&nameDesc);
			Add_Child(nameTex, L"nameTex", false);
			m_nameTex = nameTex;

		}

		{
			CUIText::TextDesc amountTexDesc = {};
			amountTexDesc.fontTag = L"Noto_Sans_CJK_SC";
			amountTexDesc.text = L"amount";
			amountTexDesc.fontAlign = CUIText::EFontAlign::RIGHT;
			shared_ptr<CUIText> amountTex = CUIText::Create(m_device, m_context);
			amountTex->Initialize(&amountTexDesc);
			Add_Child(amountTex, L"amountTex", false);
			m_amountTex = amountTex;

		}
		{
			CUIText::TextDesc loacationDesc = {};
			loacationDesc.fontTag = L"Noto_Sans_CJK_SC";
			loacationDesc.text = L"loaction";
			loacationDesc.fontAlign = CUIText::EFontAlign::RIGHT;
			shared_ptr<CUIText> locationTex = CUIText::Create(m_device, m_context);
			locationTex->Initialize(&loacationDesc);
			Add_Child(locationTex, L"locationTex", false);
			m_locationTex = locationTex;

		}
	}
	//////////////////////////////////////////////////////////

	CUIImage::UIImageDesc SizeCircleInDesc = {};
	SizeCircleInDesc.textureComLevel = ETOI(ELevel::STATIC);
	SizeCircleInDesc.textureProtoName = L"Prototype_Component_Texture_InnerTargetUI";
	shared_ptr<CUIImage> SizeCircleIn = CUIImage::Create(m_device, m_context);
	SizeCircleIn->Initialize(&SizeCircleInDesc);


	Add_Child(SizeCircleIn, L"SizeCircleIn", false);
	m_sizeCircleIn = SizeCircleIn;

	{
		CUIImage::UIImageDesc SizeCircleOutDesc = {};
		SizeCircleOutDesc.textureComLevel = ETOI(ELevel::STATIC);
		SizeCircleOutDesc.textureProtoName = L"Prototype_Component_Texture_OuterTargetUI";
		shared_ptr<CUIImage> SizeCircleOut = CUIImage::Create(m_device, m_context);
		SizeCircleOut->Initialize(&SizeCircleOutDesc);


		SizeCircleIn->Add_Child(SizeCircleOut, L"SizeCircleOut", false);
		//m_pSizeCircleOut = SizeCircleIn;

		{

			CUIImage::UIImageDesc SizeCircleDesc = {};
			SizeCircleDesc.textureComLevel = ETOI(ELevel::STATIC);
			SizeCircleDesc.textureProtoName = L"Prototype_Component_Texture_DiamondMinigame";
			shared_ptr<CUIImage> SizeCircle = CUIImage::Create(m_device, m_context);
			SizeCircle->Initialize(&SizeCircleDesc);

			SizeCircle->Set_UseColorMix(true);
			SizeCircle->Set_ColorMix(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });
			//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			SizeCircleOut->Add_Child(SizeCircle, L"SizeCircleEff", false);
			m_sizeCircle = SizeCircle;
			m_circleEff->Set_Zorder(4);
			//m_CircleShader = dynamic_pointer_cast<CShader>(CircleEff->Get_Component(L"Com_Shader"));

		}
	}

	///////////////////////////////////

	CUIImage::UIImageDesc BallCircleInDesc = {};
	BallCircleInDesc.textureComLevel = ETOI(ELevel::STATIC);
	BallCircleInDesc.textureProtoName = L"Prototype_Component_Texture_InnerTargetUI";
	BallCircleInDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> BallCircleIn = CUIImage::Create(m_device, m_context);
	BallCircleIn->Initialize(&BallCircleInDesc);


	Add_Child(BallCircleIn, L"BallCircleIn", false);
	m_ballCircle = BallCircleIn;

	{
		CUIImage::UIImageDesc BallStartDesc = {};
		BallStartDesc.textureComLevel = ETOI(ELevel::STATIC);
		BallStartDesc.textureProtoName = L"Prototype_Component_Texture_OuterTargetUI";
		BallStartDesc.shaderType = CUIRenderable::UIShaderType::Radial;
		shared_ptr<CUIImage> BallStart = CUIImage::Create(m_device, m_context);
		BallStart->Initialize(&BallStartDesc);


		BallCircleIn->Add_Child(BallStart, L"BallStart", false);
		//m_pSizeCircleOut = SizeCircleIn;




		CUIImage::UIImageDesc BallDesc = {};
		BallDesc.textureComLevel = ETOI(ELevel::STATIC);
		BallDesc.textureProtoName = L"Prototype_Component_Texture_DiamondMinigame";
		shared_ptr<CUIImage> Ball = CUIImage::Create(m_device, m_context);
		Ball->Initialize(&BallDesc);
		
		//CircleEff->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
		//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

		BallCircleIn->Add_Child(Ball, L"Ball", false);
		m_ball = Ball;

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
		m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
		m_circleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

	}
	else
	{

		m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
		m_circleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
		m_circle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
		m_changeColor = true;
		colortime = .5f;
	}
}


void CUI_MiniGame::OnActive()
{

	m_spinner->GetUITransform()->SetRotation(0);
	m_prograssBar01 = 0;
	m_isFinished = false;



	CUIPanel::OnActive();
}

void CUI_MiniGame::OnInActive()
{
	m_defId = kIdAbsence;
	CUIPanel::OnInActive();
}

void CUI_MiniGame::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_MiniGame::OnUpdate(const _float& timeDelta)
{


	if (!m_logic)
		return;


	switch(m_miniGameState)
	{
	case EMiniGameType::BASIC_CIRCLE:
		{
		m_zoneCount = m_logic->GetZoneCount();
		memcpy(m_zones, m_logic->GetZones(), sizeof(Zone) * m_zoneCount);

		

		auto result = m_logic->ConsumeInputResult();
		if (result == EInputResult::SUCCESS)
		{
			// 초록 효과
			m_prograssBar01 += 0.2;
			//m_pGameInstance.lock()->Play_Once(L"Fishing_Success");

			m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			m_circleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));
		
		}
		else if (result == EInputResult::FAIL)
		{
			//m_pGameInstance.lock()->Play_Once(L"Fishing_Failure");

			m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
			m_circleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
			m_circle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
			m_changeColor = true;
			colortime = .5f;
			// 빨강 효과
		}

		m_spinner->GetUITransform()->SetRotation(-(m_logic->GetAngle()));

		
		if (m_changeColor == true)
		{
			m_accTime2 += timeDelta;
			if (m_accTime2 >= colortime)
			{
				m_accTime2 = 0;
				m_changeColor = false;
				m_circle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			}
		}

		}

		break;
	case EMiniGameType::DIAMOND:
		{
	

			auto result = m_logic->ConsumeInputResult();

		
			m_sizeCircle->GetUITransform()->SetLocalScale(_float2{ m_logic->GetSize(), m_logic->GetSize() });
		m_sizeCircle->GetUITransform()->SetRotation(m_logic->GetAngle());
		
			/*if(m_Logic->GetSize() > 1.f)
			{
				m_pSizeCircle->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .5f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));

			}*/
			if(result == EInputResult::START)
			{
				m_sizeCircle->UI_Active();
;				//m_pSizeCircle->GetUITransform()->SetLocalScale(_float2{ 0.f,0.f });
				m_sizeCircle->m_behavior.clear();
				m_sizeCircle->Set_Alpha(1.f);

				m_sizeCircle->Set_ColorMix(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });
			}

		if (result == EInputResult::SUCCESS)
		{
			// 초록 효과
			m_prograssBar01 += 0.2;
			m_sizeCircle->m_behavior.clear();
			m_sizeCircle->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, .5f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, true));
			m_sizeCircle->Set_ColorMix(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			//m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));
			m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT,1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			LOG(ELogLevel::INFO, "SUCCES");
		}
		else if (result == EInputResult::FAIL)
		{
			m_sizeCircle->m_behavior.clear();
			m_sizeCircle->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT,.5f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, true));
			m_sizeCircle->Set_ColorMix(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
			//
			m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
			m_changeColor = true;
			colortime = .5f;
			// 빨강 효과
		}

		if (m_changeColor == true)
		{
			m_accTime2 += timeDelta;
			if (m_accTime2 >= colortime)
			{
				m_accTime2 = 0;
				m_changeColor = false;
				//m_pCircle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			}
		}
		
		}

		break;
	case EMiniGameType::BALL:

		auto result = m_logic->ConsumeInputResult();
		if (result == EInputResult::SUCCESS)
		{
			// 초록 효과
			m_prograssBar01 += 0.2;

			m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
			m_circleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

		}
		else if (result == EInputResult::FAIL)
		{
			m_circleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f }, false));
			m_circleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
			m_circle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
			m_changeColor = true;
			colortime = .5f;
			// 빨강 효과
		}
		break;
	}

	float progress = m_logic->GetProgress01();
	m_progressIcon->GetUITransform()->SetAnchoredPos(
		_float2{
			m_progressIcon->GetUITransform()->Get_AnchoredPos().x,
			lerp(0.f, 239.f, progress)
		}
	);

	m_amountTex->Set_Text(format(L"{}", m_logic->GetFishCount()));
	


	CUIPanel::OnUpdate(timeDelta);
}

void CUI_MiniGame::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_MiniGame::OnRender()
{
	if (m_circleShader != nullptr)
	{
		m_circleShader->Bind_RawValue("g_zones", m_zones, sizeof(_float4) * 8);
		m_circleShader->Bind_RawValue("g_zoneCount", &m_zoneCount, sizeof(float));

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
