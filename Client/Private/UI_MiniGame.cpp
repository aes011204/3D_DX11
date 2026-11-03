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

CUI_MiniGame::CUI_MiniGame(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CUIPanel(device, context)
{
}

CUI_MiniGame::CUI_MiniGame(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

void CUI_MiniGame::UI_PanelActive()
{
	_uint  m_defID = m_logic->Get_DefID();
	if (m_defID == kIDAbsence)
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

	ItemDef def = CItemDB::GetInstance()->GetItemByID(m_defID);

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

HRESULT CUI_MiniGame::OnInit(void* arg)
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
	MiniGamePanelDesc* panelDesc = static_cast<MiniGamePanelDesc*>(arg);
	//TABCONTAINER_DESC pDesc = {};
	panelDesc->textureComLevel = ETOI(ELevel::STATIC);
	panelDesc->textureProtoName = L"Prototype_Component_Texture_MinigamePanel";



	hr = CUIPanel::OnInit(panelDesc);


	CUIImage::UIImageDesc borderDesc = {};
	borderDesc.textureComLevel = ETOI(ELevel::STATIC);
	borderDesc.textureProtoName = L"Prototype_Component_Texture_FishingUIBorders";
	shared_ptr<CUIImage> Border = CUIImage::Create(m_device, m_context);
	Border->Initialize(&borderDesc);
	m_base = Border;
	Add_Child(Border, L"Border", false);

	CUIImage::UIImageDesc circleDesc = {};
	circleDesc.textureComLevel = ETOI(ELevel::STATIC);
	circleDesc.textureProtoName = L"Prototype_Component_Texture_FishingUICircle";
	circleDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> Circle = CUIImage::Create(m_device, m_context);
	Circle->Initialize(&circleDesc);
	Circle->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
	Circle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

	Add_Child(Circle, L"Circle", false);
	m_circle = Circle;
	m_circleShader = dynamic_pointer_cast<CShader>(Circle->Get_Component(L"Com_Shader"));


	{
		CUIImage::UIImageDesc circleEffDesc = {};
		circleEffDesc.textureComLevel = ETOI(ELevel::STATIC);
		circleEffDesc.textureProtoName = L"Prototype_Component_Texture_FishingUICircle";
		shared_ptr<CUIImage> circleEff = CUIImage::Create(m_device, m_context);
		circleEff->Initialize(&circleEffDesc);
		//CircleEff->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
		//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

		Circle->Add_Child(circleEff, L"CircleEff", false);
		m_circleEff = circleEff;
		m_circleEff->Set_Alpha(0);
		m_circleEff->Set_Zorder(4);
		//m_CircleShader = dynamic_pointer_cast<CShader>(CircleEff->Get_Component(L"Com_Shader"));
	}


	CUIImage::UIImageDesc spinnerDesc = {};
	spinnerDesc.textureComLevel = ETOI(ELevel::STATIC);
	spinnerDesc.textureProtoName = L"Prototype_Component_Texture_FishingUISpinner";
	shared_ptr<CUIImage> spinner = CUIImage::Create(m_device, m_context);
	spinner->Initialize(&spinnerDesc);
	m_spinner = spinner;
	Add_Child(spinner, L"Spinner", false);

	CUIImage::UIImageDesc fishKindIconDesc = {};
	fishKindIconDesc.textureComLevel = ETOI(ELevel::STATIC);
	fishKindIconDesc.textureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
	shared_ptr<CUIImage> fishKindIcon = CUIImage::Create(m_device, m_context);
	fishKindIcon->Initialize(&fishKindIconDesc);
	fishKindIcon->Set_UseColorMix(true);
	fishKindIcon->Set_ColorMix(_float4{ 1.f,1.f,1.f,1.f });

	Circle->Add_Child(fishKindIcon, L"FishKindIcon", false);
	m_fishIcon = fishKindIcon;



	CUIImage::UIImageDesc depthBarDesc = {};
	depthBarDesc.textureComLevel = ETOI(ELevel::STATIC);
	depthBarDesc.textureProtoName = L"Prototype_Component_Texture_CrabPotDepthBar";
	shared_ptr<CUIImage> depthBar = CUIImage::Create(m_device, m_context);
	depthBar->Initialize(&depthBarDesc);
	m_depth = depthBar;
	Add_Child(depthBar, L"DepthBar", false);

	CUIImage::UIImageDesc fishIconDesc = {};
	fishIconDesc.textureComLevel = ETOI(ELevel::STATIC);
	fishIconDesc.textureProtoName = L"Prototype_Component_Texture_FishingFishIcon";
	shared_ptr<CUIImage> fishIcon = CUIImage::Create(m_device, m_context);
	fishIcon->Initialize(&fishIconDesc);

	depthBar->Add_Child(fishIcon, L"FishIcon", false);
	m_progressIcon = fishIcon;



	/// 버튼
	CUIButton::UIButtonDesc butDesc = {};
	butDesc.textureComLevel = ETOI(ELevel::STATIC);
	butDesc.textureProtoName = L"Prototype_Component_Texture_Button_RED";
	butDesc.useDark =true;

	butDesc.OverlapStartEvent = [this](CUIButton* self)
	{
			m_gameInstance.lock()->Play_Once(L"Hover");

	};
	butDesc.OverlapEndEvent = [](CUIButton* self) {};
	butDesc.ClickEvent = [this](CUIButton* self)
		{
			if (m_logic)
				m_logic->OnInput();

			//m_pGameInstance.lock()->Play_Loop(L"fishing_loop");
			
		};
	shared_ptr<CUIButton> button = CUIButton::Create(m_device, m_context);
	button->Initialize(&butDesc);

	button->Set_SelectState(false);
	m_button = button;
	{
		CUIText::TextDesc textDesc = {};
		textDesc.fontTag = L"Noto_Sans_CJK_SC_24";
		textDesc.text = L"당기기";
		shared_ptr<CUIText> textStorage = CUIText::Create(m_device, m_context);
		textStorage->Initialize(&textDesc);
		button->Add_Child(textStorage, L"Text_Pull", false);
	}

	//Add_Child(pChild, NameTag, false);
	wstring nameTag = L"BUTTON";
	Add_Layout_Child(button, nameTag, false);






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

	CUIImage::UIImageDesc sizeCircleInDesc = {};
	sizeCircleInDesc.textureComLevel = ETOI(ELevel::STATIC);
	sizeCircleInDesc.textureProtoName = L"Prototype_Component_Texture_InnerTargetUI";
	shared_ptr<CUIImage> sizeCircleIn = CUIImage::Create(m_device, m_context);
	sizeCircleIn->Initialize(&sizeCircleInDesc);


	Add_Child(sizeCircleIn, L"SizeCircleIn", false);
	m_sizeCircleIn = sizeCircleIn;

	{
		CUIImage::UIImageDesc sizeCircleOutDesc = {};
		sizeCircleOutDesc.textureComLevel = ETOI(ELevel::STATIC);
		sizeCircleOutDesc.textureProtoName = L"Prototype_Component_Texture_OuterTargetUI";
		shared_ptr<CUIImage> sizeCircleOut = CUIImage::Create(m_device, m_context);
		sizeCircleOut->Initialize(&sizeCircleOutDesc);


		sizeCircleIn->Add_Child(sizeCircleOut, L"SizeCircleOut", false);
		//m_pSizeCircleOut = SizeCircleIn;

		{

			CUIImage::UIImageDesc sizeCircleDesc = {};
			sizeCircleDesc.textureComLevel = ETOI(ELevel::STATIC);
			sizeCircleDesc.textureProtoName = L"Prototype_Component_Texture_DiamondMinigame";
			shared_ptr<CUIImage> sizeCircle = CUIImage::Create(m_device, m_context);
			sizeCircle->Initialize(&sizeCircleDesc);

			sizeCircle->Set_UseColorMix(true);
			sizeCircle->Set_ColorMix(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });
			//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

			sizeCircleOut->Add_Child(sizeCircle, L"SizeCircleEff", false);
			m_sizeCircle = sizeCircle;
			m_circleEff->Set_Zorder(4);
			//m_CircleShader = dynamic_pointer_cast<CShader>(CircleEff->Get_Component(L"Com_Shader"));

		}
	}

	///////////////////////////////////

	CUIImage::UIImageDesc ballCircleInDesc = {};
	ballCircleInDesc.textureComLevel = ETOI(ELevel::STATIC);
	ballCircleInDesc.textureProtoName = L"Prototype_Component_Texture_InnerTargetUI";
	ballCircleInDesc.shaderType = CUIRenderable::UIShaderType::Radial;
	shared_ptr<CUIImage> ballCircleIn = CUIImage::Create(m_device, m_context);
	ballCircleIn->Initialize(&ballCircleInDesc);


	Add_Child(ballCircleIn, L"BallCircleIn", false);
	m_ballCircle = ballCircleIn;

	{
		CUIImage::UIImageDesc ballStartDesc = {};
		ballStartDesc.textureComLevel = ETOI(ELevel::STATIC);
		ballStartDesc.textureProtoName = L"Prototype_Component_Texture_OuterTargetUI";
		ballStartDesc.shaderType = CUIRenderable::UIShaderType::Radial;
		shared_ptr<CUIImage> ballStart = CUIImage::Create(m_device, m_context);
		ballStart->Initialize(&ballStartDesc);


		ballCircleIn->Add_Child(ballStart, L"BallStart", false);
		//m_pSizeCircleOut = SizeCircleIn;




		CUIImage::UIImageDesc ballDesc = {};
		ballDesc.textureComLevel = ETOI(ELevel::STATIC);
		ballDesc.textureProtoName = L"Prototype_Component_Texture_DiamondMinigame";
		shared_ptr<CUIImage> ball = CUIImage::Create(m_device, m_context);
		ball->Initialize(&ballDesc);
		
		//CircleEff->Set_BaseColor(_float4{ 81 / 255.f, 50 / 255.f, 43 / 255.f, 1.f });
		//CircleEff->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

		ballCircleIn->Add_Child(ball, L"Ball", false);
		m_ball = ball;

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

void CUI_MiniGame::Is_OnZoon(_bool isOnZone)
{
	if (isOnZone == true)
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
		m_colorTime = .5f;
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
	m_defID = kIDAbsence;
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
			m_colorTime = .5f;
			// 빨강 효과
		}

		m_spinner->GetUITransform()->SetRotation(-(m_logic->GetAngle()));

		
		if (m_changeColor == true)
		{
			m_accTime2 += timeDelta;
			if (m_accTime2 >= m_colorTime)
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
			m_colorTime = .5f;
			// 빨강 효과
		}

		if (m_changeColor == true)
		{
			m_accTime2 += timeDelta;
			if (m_accTime2 >= m_colorTime)
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
			m_colorTime = .5f;
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

shared_ptr<CUI_MiniGame> CUI_MiniGame::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_MiniGame> instance(new CUI_MiniGame(device, context), [](CUI_MiniGame* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return instance;
}

void CUI_MiniGame::Free()
{
	CUIPanel::Free();
}
