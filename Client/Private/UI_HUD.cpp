#include "UI_HUD.h"

#include "UIButton.h"
#include "UIImage.h"
#include "UI_MainMenu.h"
#include "UI_Skill.h"
#include "UI_Top.h"

CUI_HUD::CUI_HUD(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_HUD::CUI_HUD(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

HRESULT CUI_HUD::OnInit(void* pArg)
{
	{
		CUI_Skill::SKILL_DESC SkillPanelDesc = {};
		SkillPanelDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		SkillPanelDesc.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
		SkillPanelDesc.IsTransparent = true;
		SkillPanelDesc.bUseDark = false;
		shared_ptr<CUI_Skill> SkillPanel = CUI_Skill::Create(m_pDevice, m_pContext);
		SkillPanel->Initialize(&SkillPanelDesc);

		wstring NameTag = L"PANNEL_" + S2W(string(magic_enum::enum_name(HUD::SKILL)));
		Add_Child(SkillPanel, NameTag, false);
	}
	{
		CUI_Top::UIPANEL_DESC TopPanelDesc = {};
		TopPanelDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		TopPanelDesc.TextureProtoName = L"Prototype_Component_Texture_Slot_Inven";
		TopPanelDesc.IsTransparent = true;
		TopPanelDesc.bUseDark = false;

		shared_ptr<CUI_Top> TopPanel = CUI_Top::Create(m_pDevice, m_pContext);
		TopPanel->Initialize(&TopPanelDesc);

		wstring NameTag = L"PANNEL_" + S2W(string(magic_enum::enum_name(HUD::TOP)));
		Add_Child(TopPanel, NameTag, false);
	}
	{
		CUIButton::UIBUTTON_DESC BoatButtonDesc = {};
		BoatButtonDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		BoatButtonDesc.TextureProtoName = L"Prototype_Component_Texture_Speaker_Button_Default";
		//BoatButtonDesc.bUseDark = false;

		shared_ptr<CUIPanel> BoatButton = CUIPanel::Create(m_pDevice, m_pContext);
		BoatButton->Initialize(&BoatButtonDesc);

		{
			

			CUIImage::UIIMAGE_DESC img_k = {};
			img_k.TextureComLevel = ETOI(LEVEL::STATIC);
			img_k.TextureProtoName = L"Prototype_Component_Texture_KEY";
			img_k.TextureIndex = 196;
			shared_ptr<CUIPanel> Image_K = CUIPanel::Create(m_pDevice, m_pContext);
			Image_K->Initialize(&img_k);

			BoatButton->Add_Child(Image_K, L"Img_tab", false);


			CUIImage::UIIMAGE_DESC img_B = {};
			img_B.TextureComLevel = ETOI(LEVEL::STATIC);
			img_B.TextureProtoName = L"Prototype_Component_Texture_ShipIcon";
			shared_ptr<CUIPanel> Image_B = CUIPanel::Create(m_pDevice, m_pContext);
			Image_B->Initialize(&img_B);

			BoatButton->Add_Child(Image_B, L"Img_boat", false);

		}
		wstring NameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(HUD::BOAT)));
		Add_Child(BoatButton, NameTag, false);
	}

	return __super::OnInit(pArg);
}

void CUI_HUD::OnActive()
{
	__super::OnActive();
}

void CUI_HUD::OnInActive()
{
	__super::OnInActive();
}

void CUI_HUD::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_HUD::OnUpdate(const _float& timeDelta)
{
	__super::OnUpdate(timeDelta);
}

void CUI_HUD::OnLateUpdate()
{
	__super::OnLateUpdate();
}

HRESULT CUI_HUD::OnRender()
{
	return __super::OnRender();
}

void CUI_HUD::OnClear()
{
	__super::OnClear();
}

shared_ptr<CUI_HUD> CUI_HUD::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_HUD> pInstance(new CUI_HUD(pDevice, pContext), [](CUI_HUD* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_HUD");

	}
	return pInstance;
}

void CUI_HUD::Free()
{
	__super::Free();
}
