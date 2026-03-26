#include "UI_NPC.h"

#include <UIImage.h>
#include <UIText.h>

#include "FadeModifier.h"
#include "TransformModifier.h"

CUI_NPC::CUI_NPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CUI_NPC::CUI_NPC(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

HRESULT CUI_NPC::OnInit(void* pArg)
{

	//Npcimage
	{
		CUIImage::UIIMAGE_DESC NPCBaseDesc = {};
		NPCBaseDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		NPCBaseDesc.TextureProtoName = L"Prototype_Component_Texture_Mayor_Background";
		shared_ptr<CUIImage> NPCBase = CUIImage::Create(m_pDevice, m_pContext);
		NPCBase->Initialize(&NPCBaseDesc);

		{

			CUIImage::UIIMAGE_DESC NPCDesc = {};
			NPCDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			NPCDesc.TextureProtoName = L"Prototype_Component_Texture_Mayor";
			shared_ptr<CUIImage> NPC = CUIImage::Create(m_pDevice, m_pContext);
			NPC->Initialize(&NPCDesc);

			NPCBase->Add_Child(NPC, L"NPC", false);
		}
	
		Add_Child(NPCBase, L"NPC_Base", false);

		m_NpcImg = NPCBase;
	}

	//dialogue
	{

		CUIImage::UIIMAGE_DESC dialogueDesc = {};
		dialogueDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		dialogueDesc.TextureProtoName = L"Prototype_Component_Texture_DialogBox";
		shared_ptr<CUIImage> Dialogue = CUIImage::Create(m_pDevice, m_pContext);
		Dialogue->Initialize(&dialogueDesc);

		{
			CUIImage::UIIMAGE_DESC nameBaseDesc = {};
			nameBaseDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			nameBaseDesc.TextureProtoName = L"Prototype_Component_Texture_TitleBackground";
			shared_ptr<CUIImage> NameBase = CUIImage::Create(m_pDevice, m_pContext);
			NameBase->Initialize(&nameBaseDesc);

			Dialogue->Add_Child(NameBase, L"NameBase", false);

			{
				CUIText::TEXT_DESC nameDesc = {};
				nameDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
				nameDesc.strText = L"시장";
				shared_ptr<CUIText> Name_Text = CUIText::Create(m_pDevice, m_pContext);
				Name_Text->Initialize(&nameDesc);

				NameBase->Add_Child(Name_Text, L"Name_Text", false);
			}
		}


		Add_Child(Dialogue, L"Dialogue", false);
		m_Dialogue = Dialogue;
	}

	return CUIPanel::OnInit(pArg);
}
void CUI_NPC::UI_NPCActive(NPC npc , _bool dialogueOrSpeech, _bool NPCImg)
{
	
	_uint numPanel = {};


	/*switch (npc)
	{
		case NPC::MAYOR:
			Change_Texture()

			break;
		case NPC::FISHMONGER:
			Change_Texture()
			break;
		case NPC::SHIPWRIGHT:
			Change_Texture()
			break;
		case NPC::TRADER:
			Change_Texture()
			break;
		case NPC::END:
			break;
	}*/
	if(dialogueOrSpeech == true)
	{
		m_Dialogue->UI_Active();


	}
	else
	{
		m_Dialogue->UI_Active();
		m_Dialogue->Set_TextureIndex(1);
		m_Dialogue->GetChildren()[0]->UI_InActive();
	}

	if (NPCImg == true)
	{
		m_NpcImg->UI_Active();
		m_NpcImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, 1.5f, true, _float4{ 0.f,0.f,0.f,0.f }));
		// 자식도 해야함
		auto& npcImg = m_NpcImg->GetChildren()[0];
		npcImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, 2.f, true, _float4{ 0.f,0.f,0.f,0.f }));
		npcImg->m_behavior.push_back(make_shared<CTransformModifier>(_float2{-30.f,0.f}, _float2{0.f,0.f}, 0.f,1.f ));
	}


	Set_ActiveForCustom();

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	OnActive();

}
void CUI_NPC::OnActive()
{
	CUIPanel::OnActive();
}

void CUI_NPC::OnInActive()
{
	CUIPanel::OnInActive();
}

void CUI_NPC::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_NPC::OnUpdate(const _float& timeDelta)
{



	CUIPanel::OnUpdate(timeDelta);
}

void CUI_NPC::OnLateUpdate()
{
	CUIPanel::OnLateUpdate();
}

HRESULT CUI_NPC::OnRender()
{
	return CUIPanel::OnRender();
}

void CUI_NPC::OnClear()
{
	CUIPanel::OnClear();
}

void CUI_NPC::ChangeToSpeech()
{
	m_Dialogue->UI_Active();
	m_Dialogue->Set_TextureIndex(1);
	m_Dialogue->GetChildren()[0]->UI_InActive();
}

shared_ptr<CUI_NPC> CUI_NPC::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUI_NPC> pInstance(new CUI_NPC(pDevice, pContext), [](CUI_NPC* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_NPC");

	}
	return pInstance;
}

void CUI_NPC::Free()
{
	CUIPanel::Free();
}
