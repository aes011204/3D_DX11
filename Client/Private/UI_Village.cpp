#include "UI_Village.h"

#include "UIText.h"

#include "UIButton.h"
#include "UICanvas.h"
#include "UIPanel.h"
#include "UIImage.h"
#include "CScaleModifier.h"
#include "IModifier.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Engine_Helper.h"

CUI_Village::CUI_Village(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_Village::CUI_Village(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}
void CUI_Village::UI_Active()
{
	CUIPanel::UI_Active();
	for (auto& it : m_Select_boat)
	{
		it->UI_InActive();
	}
	for (auto& it : m_Select)
	{
		it->UI_InActive();

	}
}
HRESULT CUI_Village::OnInit(void* pArg)
{
	//if(pArg==nullptr)

	MAINMENU_DESC* pDesc = static_cast<MAINMENU_DESC*>(pArg);
	HRESULT hr = {};
	hr = __super::OnInit(pDesc);


	// 람다 제외 전부 데이터로 수정 가능

	wstring name = {};
	VIllAGE_LOCATION eLevel = {};
	wstring tex = {};
	for (_uint i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			eLevel = VIllAGE_LOCATION::REPAIR;
			name = L"조선공";
			tex = L"Prototype_Component_Texture_MerchantIcon";
			break;
		case 1:
			eLevel = VIllAGE_LOCATION::FISH;
			name = L"생선장수";
			tex = L"Prototype_Component_Texture_FishIconVill";
			break;
		case 2:
			eLevel = VIllAGE_LOCATION::STORAGE;
			name = L"내창고";
			tex = L"Prototype_Component_Texture_StorageIcon";
			break;
			/*		case 3:
						eLevel = VIllAGE_LOCATION::BOAT;
						name = L"";

						break*/;
		}

		CUIImage::UIIMAGE_DESC underBar_desc{};
		underBar_desc.TextureComLevel = ETOI(LEVEL::STATIC);
		underBar_desc.TextureProtoName = L"Prototype_Component_Texture_VillageUnderBar";
		shared_ptr<CUIImage> underBar = CUIImage::Create(m_pDevice, m_pContext);
		underBar->Initialize(&underBar_desc);

		wstring NameTag = L"UnderBar_" + S2W(string(magic_enum::enum_name(eLevel)));
		Add_Child(underBar, NameTag, false);


		{


			CUIButton::UIBUTTON_DESC pDesc = {};
			pDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			pDesc.TextureProtoName = L"Prototype_Component_Texture_Button";
			pDesc.TypeIndex = i;
			pDesc.OverlapStartEvent = [&](CUIButton* pThis) {auto& ch = pThis->GetChildren();
			//for (auto& it : ch)
			//{
				//if (!it || nullptr != dynamic_pointer_cast<CUIText>(it)) continue;
				m_Select[pThis->Get_TypeIndex()]->UI_Active();
				//auto pTransform = dynamic_cast<CUITransform*>(it->Get_Component(g_strUITransformTag).get());
				//if (pTransform) {
				//	pTransform->SetLocalScale({ 3.f, 1.8f });
				//}
				_float2 tmp = { 4.f, 2.f };
				m_Select[pThis->Get_TypeIndex()]->m_behavior.push_back((make_shared<CScaleModifier>(0.05f, 1.f, 0.f, tmp)));
			//}
				};
			pDesc.OverlapEndEvent = [&](CUIButton* pThis) {auto& ch = pThis->GetChildren();
			//for (auto& it : ch)
			//{
				//if (!it || nullptr != dynamic_pointer_cast<CUIText>(it)) continue;
				m_Select[pThis->Get_TypeIndex()]->UI_InActive();
				m_Select[pThis->Get_TypeIndex()]->m_behavior.clear();
			//}
				};
			pDesc.ClickEvent = [this, i, eLevel](CUIButton* pThis) {

				};
			shared_ptr<CUIButton> pBut = CUIButton::Create(m_pDevice, m_pContext);
			pBut->Initialize(&pDesc);

			//
			{
				CUIImage::UIIMAGE_DESC selectImage{};
				selectImage.TextureComLevel = ETOI(LEVEL::STATIC);
				selectImage.TextureProtoName = L"Prototype_Component_Texture_Select";
				selectImage.PxSliceLRTB = _float4{ 46.f,46.f,22.f,22.f };
				selectImage.bUseNineSlice = true;
				shared_ptr<CUIImage> Select = CUIImage::Create(m_pDevice, m_pContext);
				Select->Initialize(&selectImage);
				Select->UI_InActive();
				wstring NameTag1 = L"Select_" + S2W(string(magic_enum::enum_name(eLevel)));
				pBut->Add_Child(Select, NameTag1, false);
				m_Select[i] = Select;




				CUIText::TEXT_DESC text_Name = {};
				text_Name.strFontTag = L"Noto_Sans_CJK_SC";
				text_Name.strText = name;
				shared_ptr<CUIText> Text_Name = CUIText::Create(m_pDevice, m_pContext);
				Text_Name->Initialize(&text_Name);

				pBut->Add_Child(Text_Name, L"text_" + S2W(string(magic_enum::enum_name(eLevel))), false);


				CUIImage::UIIMAGE_DESC iconImage{};
				iconImage.TextureComLevel = ETOI(LEVEL::STATIC);
				iconImage.TextureProtoName = tex;
				shared_ptr<CUIImage> icon = CUIImage::Create(m_pDevice, m_pContext);
				icon->Initialize(&iconImage);

				wstring NameTag2 = L"icon_" + S2W(string(magic_enum::enum_name(eLevel)));
				pBut->Add_Child(icon, NameTag2, false);

			}

		NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(eLevel)));
		underBar->Add_Child(pBut, NameTag, false);
		}


	}

	CUIImage::UIIMAGE_DESC underBar_desc{};
	underBar_desc.TextureComLevel = ETOI(LEVEL::STATIC);
	underBar_desc.TextureProtoName = L"Prototype_Component_Texture_VillageUnderBar";
	shared_ptr<CUIImage> underBar = CUIImage::Create(m_pDevice, m_pContext);
	underBar->Initialize(&underBar_desc);

	Add_Child(underBar, L"UnderBar_Boat", false);


	//


	////////

	for (_uint i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			tex = L"Prototype_Component_Texture_UndockIcon";
			break;
		case 1:
			tex = L"Prototype_Component_Texture_SleepIcon";
			break;
		case 2:
			tex = L"Prototype_Component_Texture_cog_icon";
			break;
		}

		CUIButton::UIBUTTON_DESC pDesc = {};
		pDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		pDesc.TextureProtoName = L"Prototype_Component_Texture_Button";
		pDesc.TypeIndex =i;
		pDesc.OverlapStartEvent = [&](CUIButton* pThis) {auto& ch = pThis->GetChildren();
		/*for (auto& it : ch)
		{*/
			/*if (!it || nullptr != dynamic_pointer_cast<CUIText>(it)) continue;*/
			m_Select_boat[pThis->Get_TypeIndex()]->UI_Active();
			/*auto pTransform = dynamic_cast<CUITransform*>(it->Get_Component(g_strUITransformTag).get());
			if (pTransform) {
				pTransform->SetLocalScale({ .5f, .5f });
			}*/
			_float2 tmp = { 1.3f, 3.6f };
			//it->m_bIsDirtyCom = true;
			m_Select_boat[pThis->Get_TypeIndex()]->m_behavior.push_back((make_shared<CScaleModifier>(0.05f, 1.f, 0.f, tmp)));
			m_Select_boat[pThis->Get_TypeIndex()]->Set_Zorder(4);
		/*}*/
			};
		pDesc.OverlapEndEvent = [&](CUIButton* pThis) {auto& ch = pThis->GetChildren();
		//for (auto& it : ch)
		//{
			//if (!it || nullptr != dynamic_pointer_cast<CUIText>(it)) continue;
			m_Select_boat[pThis->Get_TypeIndex()]->UI_InActive();
			m_Select_boat[pThis->Get_TypeIndex()]->m_behavior.clear();
			m_Select_boat[pThis->Get_TypeIndex()]->Set_Zorder(2);
		/*}*/
			};
		pDesc.ClickEvent = [this](CUIButton* pThis) {

			};
		shared_ptr<CUIButton> BoatOP = CUIButton::Create(m_pDevice, m_pContext);
		BoatOP->Initialize(&pDesc);
		BoatOP->Set_ColorFlat(_float4{0.f,0.f,0.f,1.f});
		underBar->Add_Child(BoatOP, L"BoatOP" + to_wstring(i), false);

		{
			CUIImage::UIIMAGE_DESC selectImage{};
			selectImage.TextureComLevel = ETOI(LEVEL::STATIC);
			selectImage.TextureProtoName = L"Prototype_Component_Texture_Select";
			selectImage.PxSliceLRTB = _float4{ 46.f,46.f,22.f,22.f };
			selectImage.bUseNineSlice = true;
			shared_ptr<CUIImage> Select = CUIImage::Create(m_pDevice, m_pContext);
			Select->Initialize(&selectImage);
			Select->UI_InActive();
			Select->Set_Zorder(4);
			wstring NameTag1 = L"select_" + to_wstring(i);
			/*BoatOP->*/underBar->Add_Child(Select, NameTag1, false);
			m_Select_boat[i] = Select;


			CUIImage::UIIMAGE_DESC iconImage{};
			iconImage.TextureComLevel = ETOI(LEVEL::STATIC);
			iconImage.TextureProtoName = tex;
			shared_ptr<CUIImage> icon = CUIImage::Create(m_pDevice, m_pContext);
			icon->Initialize(&iconImage);
			
			wstring NameTag2 = L"icon_" + to_wstring(i);
			BoatOP->Add_Child(icon, NameTag2, false);
		}
	}



	CUIImage::UIIMAGE_DESC Title_desc{};
	Title_desc.TextureComLevel = ETOI(LEVEL::STATIC);
	Title_desc.TextureProtoName = L"Prototype_Component_Texture_TitleBackground";
	shared_ptr<CUIImage> Title = CUIImage::Create(m_pDevice, m_pContext);
	Title->Initialize(&Title_desc);
	Add_Child(Title, L"Title", false);

	CUIText::TEXT_DESC textVill_Name = {};
	textVill_Name.strFontTag = L"Noto_Sans_CJK_SC_32";
	textVill_Name.strText = L"큰골마을";
	shared_ptr<CUIText> TextVill = CUIText::Create(m_pDevice, m_pContext);
	TextVill->Initialize(&textVill_Name);

	Title->Add_Child(TextVill, L"text_Vill", false);



	return hr;
}

void CUI_Village::OnActive()
{
	__super::OnActive();
	
}

void CUI_Village::OnInActive()
{
	__super::OnInActive();
}

void CUI_Village::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_Village::OnUpdate(const _float& timeDelta)
{
	//for (auto& it : m_Select_boat)
	//{
	//	it->Set_Zorder(4);
	//}

	__super::OnUpdate(timeDelta);
}

void CUI_Village::OnLateUpdate()
{
	__super::OnLateUpdate();

}

HRESULT CUI_Village::OnRender()
{
	HRESULT hr = {};
	hr = __super::OnRender();

	return hr;
}

void CUI_Village::OnClear()
{
	__super::OnClear();

}



shared_ptr<CUI_Village> CUI_Village::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Village> pInstance(new CUI_Village(pDevice, pContext), [](CUI_Village* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CUI_Village::Free()
{
	__super::Free();
}
