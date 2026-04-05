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
	m_DefID = Defid;


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

	m_Speed = 100.f;
	m_RodSpeed = .1f;


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
	ButDesc.bUseDark = false;
	ButDesc.OverlapStartEvent = [](CUIButton* pThis) {};
	ButDesc.OverlapEndEvent = [](CUIButton* pThis) {};
	ButDesc.ClickEvent = [this](CUIButton* pThis)
		{
			if (!m_bStart) { m_bStart = true; m_AccTime = 0.f; }
			else {}
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

	////////////////////TEST//////////////////////////////
	m_zoneCount = 3;
	m_zones[0].start = 0.1f;
	m_zones[0].end = 0.2f;
	m_zones[1].start = 0.3f;
	m_zones[1].end = 0.6f;
	m_zones[2].start = 0.9f;
	m_zones[2].end = .95f;

	m_FishCount = 4;

	return hr;

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
	CUIPanel::OnInActive();
}

void CUI_MiniGame::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_MiniGame::OnUpdate(const _float& timeDelta)
{
	bool m_chose = false;
	if(m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_F))
	{
		if (/*m_bFin == true &&*/ m_InvenCtrl.lock()->Is_Dragging() == true)
		{ m_bStart = false; }
		else if (m_bFin == true && m_FishCount == 0)
		{m_bStart = false;}
		else if (!m_bStart && m_InvenCtrl.lock()->Is_Dragging() != true )
		{
			m_bStart = true; m_AccTime = 0.f; m_prograssBar01 = 0.f; m_bFin = false;
		}
		else { m_chose = true; }


	}
		m_prograssBar01 = clamp(m_prograssBar01, 0.f, 1.f);

		m_PrograssIcon->GetUITransform()->SetAnchoredPos(_float2{ m_PrograssIcon->GetUITransform()->Get_AnchoredPos().x,
		lerp(0.f, 239.f, m_prograssBar01) }
		);

	float angle01={};
	if (m_bStart == true )
	{
		m_prograssBar01 += m_RodSpeed * timeDelta;
		

		////////////////////////////////////
		m_AccTime += timeDelta;
		m_Angle = m_Speed * m_AccTime;
		// 쉐이더는 오른쪽이 증가인데 로직은 + 가ㅏ 왼쪽 회전이라 바꿈 
		m_Spinner->GetUITransform()->SetRotation(-m_Angle);

		float currentAngle = fmod(m_Angle, 360.f);
		if (currentAngle < 0) currentAngle += 360.f;

		angle01 = (currentAngle / 360.f);
		//LOG_F(LOG_LEVEL::INFO, "angle %f", angle01);

		if (m_chose == true)
		{
			bool isSuccess = false;
			for (int i = 0; i < m_zoneCount; i++)
			{
				if (m_zones[i].start <= angle01 && m_zones[i].end >= angle01)
				{
					isSuccess = true;
					break;
				}
			}

			if(isSuccess == true)
			{
				// 효과 초록 원 이팩트 밖으로 커짐
			//m_bStart = false;

				m_prograssBar01 += 0.2;
				m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f },false));
				m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{1.f,1.f}, false)));

			}
			else
			{
				// 초록 영역 붉어지고 빨간 원 이팩트 밖으로 커짐 아주 짧게

				m_prograssBar01 -= 0.2;

				m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, .6f, true, _float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f },false));
				m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.25f, .8f, 0.f, _float2{ 1.f,1.f }, false)));
				m_pCircle->Set_ZoneColor(_float4{ 230 / 255.f, 46 / 255.f, 49 / 255.f, 1.f });
				m_changeColor = true;
				colortime = .5f;
			}
		}
	}

	if(m_prograssBar01 >= 1.f && !m_bFin)
	{
		//진짜 성공 반환
		m_bStart = false;
		
			m_bFin = true;
	
		m_pCircleEff->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_OUT, 1.f, true, _float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f }, false));
		m_pCircleEff->m_behavior.push_back((make_shared<CScaleModifier>(0.3f, 1.f, 0.f, _float2{ 1.f,1.f }, false)));

		Evt_GetFish  e = {};
		e.DefID = m_DefID;
		e.fishInst.size = 20;
		e.fishInst.mutation_ID = 2;
		m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_GetFish>(e);

		m_FishCount--;
	}



	if(m_changeColor == true)
	{
		m_AccTime2 += timeDelta;
		if(m_AccTime2 >= colortime)
		{
			m_AccTime2 = 0;
			m_changeColor = false;
			m_pCircle->Set_ZoneColor(_float4{ 120 / 255.f, 185 / 255.f, 120 / 255.f, 1.f });

		}
	}

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
