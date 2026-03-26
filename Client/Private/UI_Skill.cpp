#include "UI_Skill.h"

#include "UIButton.h"
#include "UIImage.h"
#include "UI_MainMenu.h"

CUI_Skill::CUI_Skill(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_Skill::CUI_Skill(const CUIPanel& prototype)
	:CUIPanel(prototype)
{
}

HRESULT CUI_Skill::OnInit(void* pArg)
{

	///////ICON////////
	{
		CUIButton::UIBUTTON_DESC SkillDesc = {};
		SkillDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		SkillDesc.TextureProtoName = L"Prototype_Component_Texture_ActionButtonMain";
		//SkillPanelDesc.IsTransparent = true;
		SkillDesc.bUseDark = false;
		shared_ptr<CUIPanel> Skill = CUIPanel::Create(m_pDevice, m_pContext);
		Skill->Initialize(&SkillDesc);

		
		Add_Child(Skill, L"Skill_Base", false);
		{
			CUIImage::UIIMAGE_DESC img_k = {};
			img_k.TextureComLevel = ETOI(LEVEL::STATIC);
			img_k.TextureProtoName = L"Prototype_Component_Texture_KEY";
			img_k.TextureIndex = 121;
			shared_ptr<CUIPanel> Image_K = CUIPanel::Create(m_pDevice, m_pContext);
			Image_K->Initialize(&img_k);

			Skill->Add_Child(Image_K, L"Skill_Key", false);



			CUIImage::UIIMAGE_DESC img_B = {};
			img_B.TextureComLevel = ETOI(LEVEL::STATIC);
			img_B.TextureProtoName = L"Prototype_Component_Texture_LightsIcon";
			shared_ptr<CUIPanel> Image_B = CUIPanel::Create(m_pDevice, m_pContext);
			Image_B->Initialize(&img_B);

			Skill->Add_Child(Image_B, L"Skill_Img", false);

		}
		Add_Child(Skill, L"Skill_Base", false);
	}

	///////E KEY//////
	{
		CUIButton::UIBUTTON_DESC E_Desc = {};
		E_Desc.TextureComLevel = ETOI(LEVEL::STATIC);
		E_Desc.TextureProtoName = L"Prototype_Component_Texture_Speaker_Button_Default";
		//BoatButtonDesc.bUseDark = false;

		shared_ptr<CUIPanel> E_Base = CUIPanel::Create(m_pDevice, m_pContext);
		E_Base->Initialize(&E_Desc);

		{
			

			CUIImage::UIIMAGE_DESC img_k = {};
			img_k.TextureComLevel = ETOI(LEVEL::STATIC);
			img_k.TextureProtoName = L"Prototype_Component_Texture_KEY";
			img_k.TextureIndex = 42;
			shared_ptr<CUIPanel> Image_K = CUIPanel::Create(m_pDevice, m_pContext);
			Image_K->Initialize(&img_k);

			E_Base->Add_Child(Image_K, L"E_Key", false);


			CUIImage::UIIMAGE_DESC img_B = {};
			img_B.TextureComLevel = ETOI(LEVEL::STATIC);
			img_B.TextureProtoName = L"Prototype_Component_RadialMenuIcon";
			shared_ptr<CUIPanel> Image_B = CUIPanel::Create(m_pDevice, m_pContext);
			Image_B->Initialize(&img_B);

			E_Base->Add_Child(Image_B, L"E_Image", false);

		}
		Add_Child(E_Base, L"E_Base", false);
	}

	return __super::OnInit(pArg);
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

shared_ptr<CUI_Skill> CUI_Skill::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_Skill> pInstance(new CUI_Skill(pDevice, pContext), [](CUI_Skill* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Skill");

	}
	return pInstance;
}

void CUI_Skill::Free()
{
	__super::Free();
}
