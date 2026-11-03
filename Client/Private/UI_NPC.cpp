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

CUI_NPC::CUI_NPC(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CUI_NPC::CUI_NPC(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

HRESULT CUI_NPC::OnInit(void* arg)
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
			m_NPCImg = NPC;
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
			shared_ptr<CUIImage> nameBase = CUIImage::Create(m_device, m_context);
			nameBase->Initialize(&nameBaseDesc);

			m_nameBase = nameBase;

			Dialogue->Add_Child(nameBase, L"NameBase", false);

			{
				CUIText::TextDesc nameDesc = {};
				nameDesc.fontTag = L"Noto_Sans_CJK_SC_24";
				nameDesc.text = L"?쒖옣";
				shared_ptr<CUIText> nameText = CUIText::Create(m_device, m_context);
				nameText->Initialize(&nameDesc);

				nameBase->Add_Child(nameText, L"Name_Text", false);
				m_nameText = nameText;
			}
		}

		CUIText::TextDesc dialogueTextDesc = {};
		dialogueTextDesc.fontTag = L"Noto_Sans_CJK_SC_24";
		dialogueTextDesc.text = L"?쒖옣";
		shared_ptr<CUIText> dialogueText = CUIText::Create(m_device, m_context);
		dialogueText->Initialize(&dialogueTextDesc);

		Dialogue->Add_Child(dialogueText, L"Dialogue_Text", false);
		m_dialogueText = dialogueText;



		Add_Child(Dialogue, L"Dialogue", false);
		m_dialogue = Dialogue;
	}
	//choice
	{
		CUIButton::UIButtonDesc butDesc1 = {};
		butDesc1.textureComLevel = ETOI(ELevel::STATIC);
		butDesc1.textureProtoName = L"Prototype_Component_Texture_Button";
		butDesc1.OverlapStartEvent = [](CUIButton* self) {};
		butDesc1.OverlapEndEvent = [](CUIButton* self) {};
		butDesc1.ClickEvent = [this](CUIButton* self)
			{
				UI_DialogueActive(false, m_cachedDialogue->lines[m_curIndex].choice.leftNextID);
			};
		shared_ptr<CUIButton> buttonUp = CUIButton::Create(m_device, m_context);
		buttonUp->Initialize(&butDesc1);
		buttonUp->UI_InActive();

		{
			CUIText::TextDesc upDesc = {};
			upDesc.fontTag = L"Noto_Sans_CJK_SC_24";
			upDesc.text = L"?쒖옣";
			shared_ptr<CUIText> dialogueText = CUIText::Create(m_device, m_context);
			dialogueText->Initialize(&upDesc);

			buttonUp->Add_Child(dialogueText, L"Up_Text", false);
			m_upText = dialogueText;
		}
		wstring nameTag = L"BUTTON_UP";
		Add_Child(buttonUp, nameTag, false);
		m_leftChoice= buttonUp;
	}
	{
		CUIButton::UIButtonDesc butDesc2 = {};
		butDesc2.textureComLevel = ETOI(ELevel::STATIC);
		butDesc2.textureProtoName = L"Prototype_Component_Texture_Button";
		butDesc2.OverlapStartEvent = [](CUIButton* self) {};
		butDesc2.OverlapEndEvent = [](CUIButton* self) {};
		butDesc2.ClickEvent = [this](CUIButton* self)
			{
				//_uint tmpindex = m_CashingDialogue->lines[m_curIndex].choice.rightNextId;
				UI_DialogueActive(false, m_cachedDialogue->lines[m_curIndex].choice.rightNextID);

			};
		shared_ptr<CUIButton> buttonDown = CUIButton::Create(m_device, m_context);
		buttonDown->Initialize(&butDesc2);
		buttonDown->UI_InActive();

		{
			CUIText::TextDesc downDesc = {};
			downDesc.fontTag = L"Noto_Sans_CJK_SC_24";
			downDesc.text = L"?쒖옣";
			shared_ptr<CUIText> downText = CUIText::Create(m_device, m_context);
			downText->Initialize(&downDesc);

			buttonDown->Add_Child(downText, L"Down_Text", false);
			m_downText = downText;

		}
		wstring nameTag = L"BUTTON_DOWN";
		Add_Child(buttonDown, nameTag, false);
		m_rightChoice = buttonDown;
	}
	return CUIPanel::OnInit(arg);
}
void CUI_NPC::UI_NPCActive( const string& dialogueID, list<string>& firstList)
{
	
	m_dialogueList = firstList;

	shared_ptr<Dialogue> dialogue = CDialogueDB::GetInstance()->GetDialogueByID(dialogueID);
	_bool dialogueOrSpeech;
	if (dialogue->charTexture == nullptr)
	{
		int i = 0;
	}
	m_cachedDialogue = dialogue;
	if (dialogue->speaker != L"")
	{
		m_NPCImg->Change_Texture(dialogue->charTexture);
		m_backImg->Change_Texture(dialogue->backTexture);


		m_NPCImg->UI_Active();
		m_NPCImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::EFade::FADE_IN, 1.5f, true, _float4{ 0.f,0.f,0.f,0.f }));
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
		m_NPCImg->UI_InActive();
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
			if(m_cachedDialogue->lines[m_curIndex].nextID >= m_cachedDialogue->lines.size())
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
			UI_DialogueActive(false, m_cachedDialogue->lines[m_curIndex].nextID);
			
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

shared_ptr<CUI_NPC> CUI_NPC::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_NPC> instance(new CUI_NPC(device, context), [](CUI_NPC* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_NPC");

	}
	return instance;
}

void CUI_NPC::Free()
{
	CUIPanel::Free();
}
