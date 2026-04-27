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
		CUIImage::UIIMAGE_DESC NPCBaseDesc = {};
		//NPCBaseDesc.TextureComLevel = ETOI(LEVEL::STATIC);
		//NPCBaseDesc.TextureProtoName = L"../Bin/Resources/Textures/NPC/Fishmonger_Background.png";
		shared_ptr<CUIImage> NPCBase = CUIImage::Create(m_pDevice, m_pContext);
		NPCBase->Initialize(&NPCBaseDesc);

		{

			CUIImage::UIIMAGE_DESC NPCDesc = {};
			//NPCDesc.TextureComLevel = ETOI(LEVEL::STATIC);
			//NPCDesc.TextureProtoName = L"../Bin/Resources/Textures/NPC/Fishmonger.png";
			shared_ptr<CUIImage> NPC = CUIImage::Create(m_pDevice, m_pContext);
			NPC->Initialize(&NPCDesc);

			NPCBase->Add_Child(NPC, L"NPC", false);
			m_NpcImg = NPC;
		}

		Add_Child(NPCBase, L"NPC_Base", false);
		m_BackImg = NPCBase;
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

			m_NameBase = NameBase;

			Dialogue->Add_Child(NameBase, L"NameBase", false);

			{
				CUIText::TEXT_DESC nameDesc = {};
				nameDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
				nameDesc.strText = L"?쒖옣";
				shared_ptr<CUIText> Name_Text = CUIText::Create(m_pDevice, m_pContext);
				Name_Text->Initialize(&nameDesc);

				NameBase->Add_Child(Name_Text, L"Name_Text", false);
				m_Name_Text = Name_Text;
			}
		}

		CUIText::TEXT_DESC dialogueTextDesc = {};
		dialogueTextDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
		dialogueTextDesc.strText = L"?쒖옣";
		shared_ptr<CUIText> Dialogue_Text = CUIText::Create(m_pDevice, m_pContext);
		Dialogue_Text->Initialize(&dialogueTextDesc);

		Dialogue->Add_Child(Dialogue_Text, L"Dialogue_Text", false);
		m_Dialogue_Text = Dialogue_Text;



		Add_Child(Dialogue, L"Dialogue", false);
		m_Dialogue = Dialogue;
	}
	//choice
	{
		CUIButton::UIBUTTON_DESC ButDesc1 = {};
		ButDesc1.TextureComLevel = ETOI(LEVEL::STATIC);
		ButDesc1.TextureProtoName = L"Prototype_Component_Texture_Button";
		ButDesc1.OverlapStartEvent = [](CUIButton* pThis) {};
		ButDesc1.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc1.ClickEvent = [this](CUIButton* pThis)
			{
				UI_DialogueActive(false, m_CashingDialogue->lines[m_CurIndex].choice.leftNextId);
			};
		shared_ptr<CUIButton> button_up = CUIButton::Create(m_pDevice, m_pContext);
		button_up->Initialize(&ButDesc1);
		button_up->UI_InActive();

		{
			CUIText::TEXT_DESC upDesc = {};
			upDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
			upDesc.strText = L"?쒖옣";
			shared_ptr<CUIText> Dialogue_Text = CUIText::Create(m_pDevice, m_pContext);
			Dialogue_Text->Initialize(&upDesc);

			button_up->Add_Child(Dialogue_Text, L"Up_Text", false);
			m_Up_Text = Dialogue_Text;
		}
		wstring NameTag = L"BUTTON_UP";
		Add_Child(button_up, NameTag, false);
		m_Leftchoice= button_up;
	}
	{
		CUIButton::UIBUTTON_DESC ButDesc2 = {};
		ButDesc2.TextureComLevel = ETOI(LEVEL::STATIC);
		ButDesc2.TextureProtoName = L"Prototype_Component_Texture_Button";
		ButDesc2.OverlapStartEvent = [](CUIButton* pThis) {};
		ButDesc2.OverlapEndEvent = [](CUIButton* pThis) {};
		ButDesc2.ClickEvent = [this](CUIButton* pThis)
			{
				//_uint tmpindex = m_CashingDialogue->lines[m_curIndex].choice.rightNextId;
				UI_DialogueActive(false, m_CashingDialogue->lines[m_CurIndex].choice.rightNextId);

			};
		shared_ptr<CUIButton> button_down = CUIButton::Create(m_pDevice, m_pContext);
		button_down->Initialize(&ButDesc2);
		button_down->UI_InActive();

		{
			CUIText::TEXT_DESC downDesc = {};
			downDesc.strFontTag = L"Noto_Sans_CJK_SC_24";
			downDesc.strText = L"?쒖옣";
			shared_ptr<CUIText> Down_Text = CUIText::Create(m_pDevice, m_pContext);
			Down_Text->Initialize(&downDesc);

			button_down->Add_Child(Down_Text, L"Down_Text", false);
			m_Down_Text = Down_Text;

		}
		wstring NameTag = L"BUTTON_DOWN";
		Add_Child(button_down, NameTag, false);
		m_Rightchoice = button_down;
	}
	return CUIPanel::OnInit(pArg);
}
void CUI_NPC::UI_NPCActive( const string& dialogueId, list<string>& Fistlist)
{
	
	m_DialogueList = Fistlist;

	shared_ptr<Dialogue> dialogue = CDialogueDB::GetInstance()->GetDialogueById(dialogueId);
	_bool dialogueOrSpeech;
	if (dialogue->Texture_Char_Path == nullptr)
	{
		int i = 0;
	}
	m_CashingDialogue = dialogue;
	if (dialogue->speaker != L"")
	{
		m_NpcImg->Change_Texture(dialogue->Texture_Char_Path);
		m_BackImg->Change_Texture(dialogue->Texture_Back_Path);


		m_NpcImg->UI_Active();
		m_NpcImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, 1.5f, true, _float4{ 0.f,0.f,0.f,0.f }));
		// ?먯떇???댁빞??
		//auto& npcImg = m_NpcImg->GetChildren()[0];
		m_BackImg->m_behavior.push_back(make_shared<CFadeModifier>(CFadeModifier::FADE::FADE_IN, 2.f, true, _float4{ 0.f,0.f,0.f,0.f }));
		m_BackImg->m_behavior.push_back(make_shared<CTransformModifier>(_float2{ -30.f,0.f }, _float2{ 0.f,0.f }, 0.f, 1.f));
		m_BackImg->UI_Active();


		dialogueOrSpeech = false;
		//?먯씠 ?꾨땶寃쎌슦
		m_Name_Text->Set_Text(dialogue->speaker);
	}
	else
	{
		//?먯씤 寃쎌슦
		m_NpcImg->UI_InActive();
		m_BackImg->UI_InActive();
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

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	OnActive();

}

void CUI_NPC::UI_DialogueActive(_bool dialogueOrSpeech, _uint curIndex)
{
	m_Leftchoice->UI_InActive();
	m_Rightchoice->UI_InActive();


	if (dialogueOrSpeech == true)
	{

		m_Dialogue->UI_Active();
		m_Dialogue->Set_TextureIndex(0);
		m_Dialogue_Text->UI_Active();
		m_Dialogue_Text->Set_Text(m_CashingDialogue->lines[curIndex].text);
		//m_Dialogue->m_behavior.push_back() ?④낵

		m_NameBase->UI_InActive();
	}
	else
	{

		m_Dialogue->UI_Active();
		m_Dialogue->Set_TextureIndex(1);
		//m_Dialogue->m_behavior.push_back() ?④낵
		m_Dialogue_Text->UI_Active();
		m_Dialogue_Text->Set_Text(m_CashingDialogue->lines[curIndex].text);

	}


	m_CurIndex = curIndex;

	if(m_CashingDialogue->lines[curIndex].hasChoices == true)
	{
		// 踰꾪듉 2媛?留뚮뱾湲??쒖꽦??
		m_Leftchoice->UI_Active();
		m_Up_Text->Set_Text(m_CashingDialogue->lines[curIndex].choice.leftText);
		m_Rightchoice->UI_Active();
		m_Down_Text->Set_Text(m_CashingDialogue->lines[curIndex].choice.rightText);

		// ?ㅼ쓬 ?몃뜳??踰덊샇 媛?몄삤湲?
	}


}

void CUI_NPC::OnActive()
{
	CUIPanel::OnActive();
}

void CUI_NPC::OnInActive()
{
	m_bFIn = false;
	m_CurIndex = 0;
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

		if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_SPACE)&& m_CashingDialogue->lines[m_CurIndex].hasChoices == false)
		{
			if(m_CashingDialogue->lines[m_CurIndex].NextID >= m_CashingDialogue->lines.size())
			{
				m_bFIn = true;
				if (m_DialogueList.empty())
				{
					return; // ?닿굔 洹몃깷 ?대┃ ?덈맂梨꾨줈 ?좎?'
				}
				m_DialogueList.pop_front();

				if(m_DialogueList.empty())
				{
					// ?대객??
					m_pGameInstance.lock()->Get_EventBus()->Publish(Evt_Dialogue_Finish{});

				}
				else
				{
				UI_NPCActive(m_DialogueList.front(), m_DialogueList);
				}


				return;
			}

			//?ㅼ쓬 ?ㅼ씠?대줈洹?
			UI_DialogueActive(false, m_CashingDialogue->lines[m_CurIndex].NextID);
			
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
