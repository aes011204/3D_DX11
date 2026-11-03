#include "UI_NPC.h"

#include <UIButton.h>
#include <UIImage.h>


#include "DialogueDB.h"
#include "FadeModifier.h"
#include "UIText.h"
#include "GameInstance.h"
#include "TransformModifier.h"
#include "DInput_Manager.h"
#include "EventBus.h"

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
		CUIImage::UIImageDesc NPCBaseDesc = {};
		//NPCBaseDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		//NPCBaseDesc.TextureProtoName = L"../Bin/Resources/Textures/NPC/Fishmonger_Background.png";
		shared_ptr<CUIImage> NPCBase = CUIImage::Create(m_device, m_context);
		NPCBase->Initialize(&NPCBaseDesc);

		{

			CUIImage::UIImageDesc NPCDesc = {};
			//NPCDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			//NPCDesc.TextureProtoName = L"../Bin/Resources/Textures/NPC/Fishmonger.png";
			shared_ptr<CUIImage> NPC = CUIImage::Create(m_device, m_context);
			NPC->Initialize(&NPCDesc);

			NPCBase->Add_Child(NPC, L"NPC", false);
			m_npcImg = NPC;
		}

		Add_Child(NPCBase, L"NPC_Base", false);
		m_backImg = NPCBase;
	}

	//dialogue
	{

		CUIImage::UIImageDesc dialogueDesc = {};
		dialogueDesc.textureComLevel = ETOI(ELevel::STATIC);
		dialogueDesc.textureProtoName = L"Prototype_Component_Texture_DialogBox";
		shared_ptr<CUIImage> Dialogue = CUIImage::Create(m_device, m_context);
		Dialogue->Initialize(&dialogueDesc);

		{
			CUIImage::UIImageDesc nameBaseDesc = {};
			nameBaseDesc.textureComLevel = ETOI(ELevel::STATIC);
			nameBaseDesc.textureProtoName = L"Prototype_Component_Texture_TitleBackground";
			shared_ptr<CUIImage> NameBase = CUIImage::Create(m_device, m_context);
			NameBase->Initialize(&nameBaseDesc);

			m_nameBase = NameBase;

			Dialogue->Add_Child(NameBase, L"NameBase", false);

			{
				CUIText::TextDesc nameDesc = {};
				nameDesc.fontTag = L"Noto_Sans_CJK_SC_24";
				nameDesc.text = L"?쒖옣";
				shared_ptr<CUIText> Name_Text = CUIText::Create(m_device, m_context);
				Name_Text->Initialize(&nameDesc);

				NameBase->Add_Child(Name_Text, L"Name_Text", false);
				m_nameText = Name_Text;
			}
		}

		CUIText::TextDesc dialogueTextDesc = {};
		dialogueTextDesc.fontTag = L"Noto_Sans_CJK_SC_24";
		dialogueTextDesc.text = L"?쒖옣";
		shared_ptr<CUIText> Dialogue_Text = CUIText::Create(m_device, m_context);
		Dialogue_Text->Initialize(&dialogueTextDesc);

		Dialogue->Add_Child(Dialogue_Text, L"Dialogue_Text", false);
		m_dialogueText = Dialogue_Text;



		Add_Child(Dialogue, L"Dialogue", false);
		m_dialogue = Dialogue;
	}
	//choice
	{
		CUIButton::UIButtonDesc ButDesc1 = {};
		ButDesc1.textureComLevel = ETOI(ELevel::STATIC);
		ButDesc1.textureProtoName = L"Prototype_Component_Texture_Button";
		ButDesc1.OverlapStartEvent = [](CUIButton* pThis) {};
		ButDesc1.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc1.ClickEvent = [this](CUIButton* pThis)
			{
				UI_DialogueActive(false, m_cachedDialogue->lines[m_curIndex].choice.leftNextId);
			};
		shared_ptr<CUIButton> button_up = CUIButton::Create(m_device, m_context);
		button_up->Initialize(&ButDesc1);
		button_up->UI_InActive();

		{
			CUIText::TextDesc upDesc = {};
			upDesc.fontTag = L"Noto_Sans_CJK_SC_24";
			upDesc.text = L"?쒖옣";
			shared_ptr<CUIText> Dialogue_Text = CUIText::Create(m_device, m_context);
			Dialogue_Text->Initialize(&upDesc);

			button_up->Add_Child(Dialogue_Text, L"Up_Text", false);
			m_upText = Dialogue_Text;
		}
		wstring NameTag = L"BUTTON_UP";
		Add_Child(button_up, NameTag, false);
		m_leftChoice= button_up;
	}
	{
		CUIButton::UIButtonDesc ButDesc2 = {};
		ButDesc2.textureComLevel = ETOI(ELevel::STATIC);
		ButDesc2.textureProtoName = L"Prototype_Component_Texture_Button";
		ButDesc2.OverlapStartEvent = [](CUIButton* pThis) {};
		ButDesc2.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc2.ClickEvent = [this](CUIButton* pThis)
			{
				//_uint tmpindex = m_CashingDialogue->lines[m_curIndex].choice.rightNextId;
				UI_DialogueActive(false, m_cachedDialogue->lines[m_curIndex].choice.rightNextId);

			};
		shared_ptr<CUIButton> button_down = CUIButton::Create(m_device, m_context);
		button_down->Initialize(&ButDesc2);
		button_down->UI_InActive();

		{
			CUIText::TextDesc downDesc = {};
			downDesc.fontTag = L"Noto_Sans_CJK_SC_24";
			downDesc.text = L"?쒖옣";
			shared_ptr<CUIText> Down_Text = CUIText::Create(m_device, m_context);
			Down_Text->Initialize(&downDesc);

			button_down->Add_Child(Down_Text, L"Down_Text", false);
			m_downText = Down_Text;

		}
		wstring NameTag = L"BUTTON_DOWN";
		Add_Child(button_down, NameTag, false);
		m_rightChoice = button_down;
	}
	return CUIPanel::OnInit(pArg);
}
void CUI_NPC::UI_NPCActive( const string& dialogueId, list<string>& Fistlist)
{
	
	m_dialogueList = Fistlist;

	shared_ptr<Dialogue> dialogue = CDialogueDB::GetInstance()->GetDialogueById(dialogueId);
	_bool dialogueOrSpeech;
	if (dialogue->Texture_Char_Path == nullptr)
	{
		int i = 0;
	}
	m_cachedDialogue = dialogue;
	if (dialogue->speaker != L"")
	{
		m_npcImg->Change_Texture(dialogue->Texture_Char_Path);
		m_backImg->Change_Texture(dialogue->Texture_Back_Path);


		m_npcImg->UI_Active();
		m_npcImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_IN, 1.5f, true, _float4{ 0.f,0.f,0.f,0.f }));
		// ?먯떇???댁빞??
		//auto& npcImg = m_NpcImg->GetChildren()[0];
		m_backImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_IN, 2.f, true, _float4{ 0.f,0.f,0.f,0.f }));
		m_backImg->m_behavior.push_back(make_shared<CTransformModifier>(_float2{ -30.f,0.f }, _float2{ 0.f,0.f }, 0.f, 1.f));
		m_backImg->UI_Active();


		dialogueOrSpeech = false;
		//?먯씠 ?꾨땶寃쎌슦
		m_nameText->Set_Text(dialogue->speaker);
	}
	else
	{
		//?먯씤 寃쎌슦
		m_npcImg->UI_InActive();
		m_backImg->UI_InActive();
		dialogueOrSpeech = true;
	}

	//if (NPCImg == true)
	//{
	//	m_NpcImg->UI_Active();
	//	m_NpcImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, 1.5f, true, _float4{ 0.f,0.f,0.f,0.f }));
	//	// ?먯떇???댁빞??
	//	auto& npcImg = m_NpcImg->GetChildren()[0];
	//	npcImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, 2.f, true, _float4{ 0.f,0.f,0.f,0.f }));
	//	npcImg->m_behavior.push_back(make_shared<CTransformModifier>(_float2{ -30.f,0.f }, _float2{ 0.f,0.f }, 0.f, 1.f));
	//}
	//m_Dialogue_Text->Set_Text(dialogue->lines[0].text);


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

	UI_DialogueActive(dialogueOrSpeech, 0);


	Set_ActiveForCustom();

	m_isRenderReady = false;

	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();

	OnActive();

}

void CUI_NPC::UI_DialogueActive(_bool dialogueOrSpeech, _uint curIndex)
{
	m_leftChoice->UI_InActive();
	m_rightChoice->UI_InActive();


	if (dialogueOrSpeech == true)
	{

		m_dialogue->UI_Active();
		m_dialogue->Set_TextureIndex(0);
		m_dialogueText->UI_Active();
		m_dialogueText->Set_Text(m_cachedDialogue->lines[curIndex].text);
		//m_Dialogue->m_behavior.push_back() ?④낵

		m_nameBase->UI_InActive();
	}
	else
	{

		m_dialogue->UI_Active();
		m_dialogue->Set_TextureIndex(1);
		//m_Dialogue->m_behavior.push_back() ?④낵
		m_dialogueText->UI_Active();
		m_dialogueText->Set_Text(m_cachedDialogue->lines[curIndex].text);

	}


	m_curIndex = curIndex;

	if(m_cachedDialogue->lines[curIndex].hasChoices == true)
	{
		// 踰꾪듉 2媛?留뚮뱾湲??쒖꽦??
		m_leftChoice->UI_Active();
		m_upText->Set_Text(m_cachedDialogue->lines[curIndex].choice.leftText);
		m_rightChoice->UI_Active();
		m_downText->Set_Text(m_cachedDialogue->lines[curIndex].choice.rightText);

		// ?ㅼ쓬 ?몃뜳??踰덊샇 媛?몄삤湲?
	}


}

void CUI_NPC::OnActive()
{
	CUIPanel::OnActive();
}

void CUI_NPC::OnInActive()
{
	m_isFinished = false;
	m_curIndex = 0;
	CUIPanel::OnInActive();
}

void CUI_NPC::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CUI_NPC::OnUpdate(const _float& timeDelta)
{
	//if (/*?ㅼ씠?대줈洹?媛 ?앸궖?꾨뻹*/)
	//{

		if (m_gameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_SPACE)&& m_cachedDialogue->lines[m_curIndex].hasChoices == false)
		{
			if(m_cachedDialogue->lines[m_curIndex].NextID >= m_cachedDialogue->lines.size())
			{
				m_isFinished = true;
				if (m_dialogueList.empty())
				{
					return; // ?닿굔 洹몃깷 ?대┃ ?덈맂梨꾨줈 ?좎?'
				}
				m_dialogueList.pop_front();

				if(m_dialogueList.empty())
				{
					// ?대객??
					m_gameInstance.lock()->Get_EventBus()->Publish(Evt_DialogueFinish{});

				}
				else
				{
				UI_NPCActive(m_dialogueList.front(), m_dialogueList);
				}


				return;
			}

			//?ㅼ쓬 ?ㅼ씠?대줈洹?
			UI_DialogueActive(false, m_cachedDialogue->lines[m_curIndex].NextID);
			
		}
	//}


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
	m_dialogue->UI_Active();
	m_dialogue->Set_TextureIndex(1);
	m_dialogue->GetChildren()[0]->UI_InActive();
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
