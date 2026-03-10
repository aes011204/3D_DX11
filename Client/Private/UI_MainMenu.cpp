#include "UI_MainMenu.h"
#include "UIButton.h"
#include "UICanvas.h"
#include "UIPanel.h"
#include "UIImage.h"
#include "CScaleModifier.h"
#include "IModifier.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Engine_Helper.h"

CUI_MainMenu::CUI_MainMenu(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUIPanel(pDevice, pContext)
{
}

CUI_MainMenu::CUI_MainMenu(const CUIPanel& prototype)
    : CUIPanel(prototype)
{
}

HRESULT CUI_MainMenu::OnInit(void* pArg)
{
	//if(pArg==nullptr)
		
    MAINMENU_DESC* pDesc = static_cast<MAINMENU_DESC*>(pArg);
	HRESULT hr = {};
	hr = __super::OnInit(pDesc);
    

	// 람다 제외 전부 데이터로 수정 가능
	{
		LEVEL eLevel = {};
		for (_uint i = 0; i < 3;i++)
		{
			switch (i)
			{
			case 0:
				eLevel = LEVEL::GAMEPLAY;
				break;
			case 1:
				eLevel = LEVEL::END;
				break;
			case 2:
				eLevel = LEVEL::EDITOR;
				break;

			}


			CUIButton::UIBUTTON_DESC pDesc = {};
			pDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			pDesc.TextureProtoName = L"Prototype_Component_Texture_Button";
			pDesc.vAnchoredPos = Vector2{ 0.f,16.7f + (98.f * i) };
			pDesc.vSizeDelta = Vector2{ 50.f,50.f };// 안건드려도 됨 텍스쳐에서 초기화 예정
			pDesc.vAnchorPoint = Vector2{ 0.254f,0.5f };
			pDesc.vPivot = Vector2{ 0.5f, 0.5f };
			pDesc.vScale = Vector2{ 1.3f, 1.0f };
			pDesc.OverlapStartEvent = [](CUIButton* pThis) {auto& ch = pThis->GetChildren();
			for (auto& it : ch)
			{
				if (!it) continue;
				it->UI_Active();
				auto pTransform = dynamic_cast<CUITransform*>(it->Get_Component(g_strUITransformTag).get());
				if (pTransform) {
					pTransform->SetLocalScale({ 1.8f, 1.8f });
				}
				//it->m_bIsDirtyCom = true;
				it->m_behavior.push_back((make_shared<CScaleModifier>(0.03f, 4.f, 0.f, 1.8f)));
			}
				};
			pDesc.OverlapEndEvent = [](CUIButton* pThis) {auto& ch = pThis->GetChildren();
			for (auto& it : ch)
			{
				if (!it) continue;
				it->UI_InActive();
				it->m_behavior.clear();
			}
				};
			pDesc.ClickEvent = [this, i, eLevel](CUIButton* pThis) {
				if(eLevel!= LEVEL::END)
				m_pGameInstance.lock()->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eLevel));
			};
			shared_ptr<CUIButton> pChild = CUIButton::Create(m_pDevice, m_pContext);
			pChild->Initialize(&pDesc);

			wstring NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(eLevel)));

			Add_Child(pChild,NameTag, false);

			

			//

			CUIImage::UIIMAGE_DESC selectImage{};
			selectImage.TextureComLevel = ETOI(LEVEL::STATIC);
			selectImage.TextureProtoName = L"Prototype_Component_Texture_Select";
			selectImage.PxSliceLRTB = _float4{ 46.f,46.f,22.f,22.f };
			selectImage.bUseNineSlice = true;
			shared_ptr<CUIImage> Select = CUIImage::Create(m_pDevice, m_pContext);
			Select->Initialize(&selectImage);
			Select->UI_InActive();

			wstring NameTag1 = L"IMGAE_SELECT" + S2W(string(magic_enum::enum_name(eLevel)));
			pChild->Add_Child(Select, NameTag1, false);
			//
			Select->Set_Zorder(2);
		}

		CUIImage::UIIMAGE_DESC image_desc{};
		image_desc.TextureComLevel = ETOI(LEVEL::STATIC);
		image_desc.TextureProtoName = L"Prototype_Component_Texture_Dredge";
		image_desc.vAnchoredPos = Vector2{ 0.f,0.f };
		image_desc.vSizeDelta = Vector2{ 50.f,50.f };// 안건드려도 됨 텍스쳐에서 초기화 예정
		image_desc.vAnchorPoint = Vector2{ 0.254f,0.319f };
		image_desc.vPivot = Vector2{ 0.5f, 0.5f };
		image_desc.vScale = Vector2{ 0.5f, 0.5f };
		shared_ptr<CUIImage> pChild2 = CUIImage::Create(m_pDevice, m_pContext);
		pChild2->Initialize(&image_desc);

		Add_Child(pChild2, L"IMAGE_DRADGELOGO", false);

		
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

shared_ptr<CUI_MainMenu> CUI_MainMenu::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CUI_MainMenu> pInstance(new CUI_MainMenu(pDevice, pContext), [](CUI_MainMenu* p) {p->Free(); delete(p); });

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CUIPanel");

    }
    return pInstance;
}

void CUI_MainMenu::Free()
{
    __super::Free();
}
