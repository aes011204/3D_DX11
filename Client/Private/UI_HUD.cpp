#include "UI_HUD.h"

#include "UIButton.h"
#include "UIImage.h"
#include "UI_MainMenu.h"
#include "UI_Skill.h"
#include "UI_Top.h"

CUI_HUD::CUI_HUD(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CUI_HUD::CUI_HUD(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

HRESULT CUI_HUD::OnInit(void* arg)
{
	{
		CUI_Skill::SkillDesc skillPanelDesc = {};
		skillPanelDesc.textureComLevel = ETOI(ELevel::STATIC);
		skillPanelDesc.textureProtoName = L"Prototype_Component_Texture_Slot_Inven";
		skillPanelDesc.isTransparent = true;
		skillPanelDesc.useDark = false;
		shared_ptr<CUI_Skill> skillPanel = CUI_Skill::Create(m_device, m_context);
		skillPanel->Initialize(&skillPanelDesc);

		wstring nameTag = L"PANNEL_" + S2W(string(magic_enum::enum_name(EHUD::SKILL)));
		Add_Child(skillPanel, nameTag, false);
	}
	{
		CUI_Top::UIPanelDesc topPanelDesc = {};
		topPanelDesc.textureComLevel = ETOI(ELevel::STATIC);
		topPanelDesc.textureProtoName = L"Prototype_Component_Texture_Slot_Inven";
		topPanelDesc.isTransparent = true;
		topPanelDesc.useDark = false;

		shared_ptr<CUI_Top> topPanel = CUI_Top::Create(m_device, m_context);
		topPanel->Initialize(&topPanelDesc);

		wstring nameTag = L"PANNEL_" + S2W(string(magic_enum::enum_name(EHUD::TOP)));
		Add_Child(topPanel, nameTag, false);
	}
	{
		CUIButton::UIButtonDesc boatButtonDesc = {};
		boatButtonDesc.textureComLevel = ETOI(ELevel::STATIC);
		boatButtonDesc.textureProtoName = L"Prototype_Component_Texture_Speaker_Button_Default";
		//BoatButtonDesc.bUseDark = false;

		shared_ptr<CUIPanel> boatButton = CUIPanel::Create(m_device, m_context);
		boatButton->Initialize(&boatButtonDesc);

		{
			

			CUIImage::UIImageDesc imgK = {};
			imgK.textureComLevel = ETOI(ELevel::STATIC);
			imgK.textureProtoName = L"Prototype_Component_Texture_KEY";
			imgK.textureIndex = 196;
			shared_ptr<CUIPanel> imageK = CUIPanel::Create(m_device, m_context);
			imageK->Initialize(&imgK);

			boatButton->Add_Child(imageK, L"Img_tab", false);


			CUIImage::UIImageDesc imgB = {};
			imgB.textureComLevel = ETOI(ELevel::STATIC);
			imgB.textureProtoName = L"Prototype_Component_Texture_ShipIcon";
			shared_ptr<CUIPanel> imageB = CUIPanel::Create(m_device, m_context);
			imageB->Initialize(&imgB);

			boatButton->Add_Child(imageB, L"Img_boat", false);

		}
		wstring nameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(EHUD::BOAT)));
		Add_Child(boatButton, nameTag, false);
	}

	return __super::OnInit(arg);
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

shared_ptr<CUI_HUD> CUI_HUD::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_HUD> instance(new CUI_HUD(device, context), [](CUI_HUD* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_HUD");

	}
	return instance;
}

void CUI_HUD::Free()
{
	__super::Free();
}
