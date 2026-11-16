#include "UI_Skill.h"

#include "UIButton.h"
#include "UIImage.h"
#include "UI_MainMenu.h"

CUI_Skill::CUI_Skill(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CUI_Skill::CUI_Skill(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

HRESULT CUI_Skill::OnInit(void* arg)
{

	///////ICON////////
	{
		CUIButton::UIButtonDesc skillDesc = {};
		skillDesc.textureComLevel = ETOI(ELevel::STATIC);
		skillDesc.textureProtoName = L"Prototype_Component_Texture_ActionButtonMain";
		//SkillPanelDesc.IsTransparent = true;
		skillDesc.useDark = false;
		shared_ptr<CUIPanel> skill = CUIPanel::Create(m_device, m_context);
		skill->Initialize(&skillDesc);

		
		Add_Child(skill, L"Skill_Base", false);
		{
			CUIImage::UIImageDesc imgK = {};
			imgK.textureComLevel = ETOI(ELevel::STATIC);
			imgK.textureProtoName = L"Prototype_Component_Texture_KEY";
			imgK.textureIndex = 121;
			shared_ptr<CUIPanel> imageK = CUIPanel::Create(m_device, m_context);
			imageK->Initialize(&imgK);

			skill->Add_Child(imageK, L"Skill_Key", false);



			CUIImage::UIImageDesc imgB = {};
			imgB.textureComLevel = ETOI(ELevel::STATIC);
			imgB.textureProtoName = L"Prototype_Component_Texture_LightsIcon";
			shared_ptr<CUIPanel> imageB = CUIPanel::Create(m_device, m_context);
			imageB->Initialize(&imgB);

			skill->Add_Child(imageB, L"Skill_Img", false);

		}
		Add_Child(skill, L"Skill_Base", false);
	}

	///////E KEY//////
	{
		CUIButton::UIButtonDesc eKeyDesc = {};
		eKeyDesc.textureComLevel = ETOI(ELevel::STATIC);
		eKeyDesc.textureProtoName = L"Prototype_Component_Texture_Speaker_Button_Default";
		//BoatButtonDesc.bUseDark = false;

		shared_ptr<CUIPanel> eKeyBase = CUIPanel::Create(m_device, m_context);
		eKeyBase->Initialize(&eKeyDesc);

		{
			

			CUIImage::UIImageDesc imgK = {};
			imgK.textureComLevel = ETOI(ELevel::STATIC);
			imgK.textureProtoName = L"Prototype_Component_Texture_KEY";
			imgK.textureIndex = 42;
			shared_ptr<CUIPanel> imageK = CUIPanel::Create(m_device, m_context);
			imageK->Initialize(&imgK);

			eKeyBase->Add_Child(imageK, L"E_Key", false);


			CUIImage::UIImageDesc imgB = {};
			imgB.textureComLevel = ETOI(ELevel::STATIC);
			imgB.textureProtoName = L"Prototype_Component_RadialMenuIcon";
			shared_ptr<CUIPanel> imageB = CUIPanel::Create(m_device, m_context);
			imageB->Initialize(&imgB);

			eKeyBase->Add_Child(imageB, L"E_Image", false);

		}
		Add_Child(eKeyBase, L"E_Base", false);
	}

	return __super::OnInit(arg);
}

void CUI_Skill::OnActive()
{
	__super::OnActive();
}

void CUI_Skill::OnInActive()
{
	__super::OnInActive();
}

void CUI_Skill::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_Skill::OnUpdate(const _float& timeDelta)
{
	__super::OnUpdate(timeDelta);
}

void CUI_Skill::OnLateUpdate()
{
	__super::OnLateUpdate();
}

HRESULT CUI_Skill::OnRender()
{
	return __super::OnRender();
}

void CUI_Skill::OnClear()
{
	__super::OnClear();
}

shared_ptr<CUI_Skill> CUI_Skill::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_Skill> instance(new CUI_Skill(device, context), [](CUI_Skill* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Skill");

	}
	return instance;
}

void CUI_Skill::Free()
{
	__super::Free();
}
