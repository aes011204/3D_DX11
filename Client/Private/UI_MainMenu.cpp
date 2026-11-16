#include "UI_MainMenu.h"

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
#include "FadeModifier.h"
#include "UI_Controller.h"
#include "EventBus.h"

CUI_MainMenu::CUI_MainMenu(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    :CUIPanel(device, context)
{
}

CUI_MainMenu::CUI_MainMenu(const CUIPanel& prototype)
    : CUIPanel(prototype)
{
}

HRESULT CUI_MainMenu::OnInit(void* arg)
{
	//if(pArg==nullptr)
		
    MainMenuDesc* desc = static_cast<MainMenuDesc*>(arg);
	HRESULT hr = {};
	hr = __super::OnInit(desc);
    

	// 람다 제외 전부 데이터로 수정 가능
	{
		wstring name = {};
		ELevel level = {};
		for (_uint i = 0; i < 3;i++)
		{
			switch (i)
			{
			case 0:
				level = ELevel::GAMEPLAY;
				name = L"새 게임";
				break;
			case 1:
				level = ELevel::END;
				name = L"설정";

				break;
			case 2:
				level = ELevel::EDITOR;
				name = L"에디터";

				break;

			}


			CUIButton::UIButtonDesc desc = {};
			desc.textureComLevel = ETOI(ELevel::STATIC);
			desc.textureProtoName = L"Prototype_Component_Texture_Button";
			desc.anchoredPos = Vector2{ 0.f,16.7f + (98.f * i) };
			desc.sizeDelta = Vector2{ 50.f,50.f };// 안건드려도 됨 텍스쳐에서 초기화 예정
			desc.anchorPoint = Vector2{ 0.254f,0.5f };
			desc.pivot = Vector2{ 0.5f, 0.5f };
			desc.scale = Vector2{ 1.3f, 1.0f };
			desc.OverlapStartEvent = [this](CUIButton* self) {auto& ch = self->GetChildren();
			for (auto& it : ch)
			{
				if (!it || nullptr != dynamic_pointer_cast<CUIText>(it)) continue;
				it->UI_Active();
				auto transform = dynamic_cast<CUITransform*>(it->Get_Component(g_strUITransformTag).get());
				if (transform) {
					transform->SetLocalScale({ 3.f, 1.8f });
				}
				_float2 tmp = { 3.7f, 2.2f };
				//it->m_bIsDirtyCom = true;
				it->m_behavior.push_back((make_shared<CScaleModifier>(0.075f, 5.f, 0.f, tmp)));
			}
			//CUI_Controller::GetInstance()->Get_LoadingUI()->m_behavior.push_back(
			//	make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, .1f, false, _float4{ 0.f,0.f,0.f,0.f }));
			m_gameInstance.lock()->Play_Once(L"Hover");

			};

			desc.OverlapEndEvent = [](CUIButton* self) {auto& ch = self->GetChildren();
			for (auto& it : ch)
			{
				if (!it || nullptr != dynamic_pointer_cast<CUIText>(it)) continue;
				it->UI_InActive();
				it->m_behavior.clear();
			}
				};

			desc.ClickEvent = [this, i, level](CUIButton* self) {
				//if (eLevel != LEVEL::END)
				//	m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eLevel));
				
				shared_ptr <CFadeModifier> eff = make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_IN, .2f, false, _float4{ 0.f,0.f,0.f,0.f });
				CUI_Controller::GetInstance()->Get_LoadingUI()->UI_Active();
				CUI_Controller::GetInstance()->Get_LoadingUI()->m_behavior.push_back(eff
				);

				//eff->SetEvent([this, i, eLevel]() {
					Evt_ChangeLevel e = {};
					e.isChange = true;
					e.level = ETOI(level);
					m_gameInstance.lock()->Get_EventBus()->Publish(e);
					//});
					m_gameInstance.lock()->Play_Once(L"Click");

			};
			shared_ptr<CUIButton> child = CUIButton::Create(m_device, m_context);
			child->Initialize(&desc);
			

			{
				CUIText::TextDesc nameTextDesc = {};
				nameTextDesc.fontTag = L"Noto_Sans_CJK_SC_24";
				nameTextDesc.text = name;
				shared_ptr<CUIText> nameText = CUIText::Create(m_device, m_context);
				nameText->Initialize(&nameTextDesc);

				child->Add_Child(nameText, L"text_Name_" + S2W(string(magic_enum::enum_name(level))), false);
			}





			wstring nameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(level)));

			Add_Child(child,nameTag, false);

			

			//

			CUIImage::UIImageDesc selectImage{};
			selectImage.textureComLevel = ETOI(ELevel::STATIC);
			selectImage.textureProtoName = L"Prototype_Component_Texture_Select";
			selectImage.pxSliceLRTB = _float4{ 46.f,46.f,22.f,22.f };
			selectImage.useNineSlice = true;
			//selectImage. = true;
			shared_ptr<CUIImage> Select = CUIImage::Create(m_device, m_context);
			Select->Initialize(&selectImage);
			Select->UI_InActive();

			wstring nameTag1 = L"IMGAE_SELECT" + S2W(string(magic_enum::enum_name(level)));
			child->Add_Child(Select, nameTag1, false);
			//
			Select->Set_Zorder(2);
		}

		CUIImage::UIImageDesc imageDesc{};
		imageDesc.textureComLevel = ETOI(ELevel::STATIC);
		imageDesc.textureProtoName = L"Prototype_Component_Texture_Dredge";
		imageDesc.anchoredPos = Vector2{ 0.f,0.f };
		imageDesc.sizeDelta = Vector2{ 50.f,50.f };// 안건드려도 됨 텍스쳐에서 초기화 예정
		imageDesc.anchorPoint = Vector2{ 0.254f,0.319f };
		imageDesc.pivot = Vector2{ 0.5f, 0.5f };
		imageDesc.scale = Vector2{ 0.5f, 0.5f };
		shared_ptr<CUIImage> child2 = CUIImage::Create(m_device, m_context);
		child2->Initialize(&imageDesc);

		Add_Child(child2, L"IMAGE_DRADGELOGO", false);

		
		//


		////////

	}


    return hr;
}

void CUI_MainMenu::OnActive()
{
    __super::OnActive();
}

void CUI_MainMenu::OnInActive()
{
    __super::OnInActive();
}

void CUI_MainMenu::OnDisabled()
{
    __super::OnDisabled();
}

void CUI_MainMenu::OnUpdate(const _float& timeDelta)
{
    __super::OnUpdate(timeDelta);
}

void CUI_MainMenu::OnLateUpdate()
{
    __super::OnLateUpdate();

}

HRESULT CUI_MainMenu::OnRender()
{
	HRESULT hr = {};
	hr = __super::OnRender();

	return hr;
}

void CUI_MainMenu::OnClear()
{
    __super::OnClear();

}

shared_ptr<CUI_MainMenu> CUI_MainMenu::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
    shared_ptr<CUI_MainMenu> instance(new CUI_MainMenu(device, context), [](CUI_MainMenu* p) {p->Free(); delete(p); });

    if (FAILED(instance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIPanel");

    }
    return instance;
}

void CUI_MainMenu::Free()
{
    __super::Free();
}
