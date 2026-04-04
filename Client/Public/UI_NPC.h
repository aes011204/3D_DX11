#pragma once
#include "UIPanel.h"

#include "Client_Define.h"

namespace Engine
{
    class CUIImage;
    class CUIText;
    class CUIButton;
}

NS_BEGIN(Client)

	class CUI_NPC final:
    public CUIPanel
{
public:
    struct NPC_DESC : public CUIPanel::UIPANEL_DESC
    {
    };
protected:
    CUI_NPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_NPC(const CUIPanel& prototype);
public:
    ~CUI_NPC() = default;

public:

    HRESULT OnInit(void* pArg) override;
    void UI_NPCActive(NPC npc, _bool dialogue, _bool NPCImg, const string& dialogueId);
    void UI_DialogueActive(_bool dialogueOrSpeech, _uint curIndex);
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;


    void ChangeToSpeech();
public:
    shared_ptr<CUIImage> m_Dialogue = { nullptr };
    shared_ptr<CUIImage> m_NpcImg = { nullptr };
    shared_ptr<CUIImage> m_BackImg = { nullptr };
    shared_ptr<CUIText> m_Name_Text = { nullptr };
    shared_ptr<CUIText> m_Dialogue_Text = { nullptr };

    shared_ptr<CUIButton>m_Rightchoice = { nullptr };
    shared_ptr<CUIButton>m_Leftchoice = { nullptr };
    shared_ptr<CUIText> m_Up_Text = { nullptr };
    shared_ptr<CUIText> m_Down_Text = { nullptr };
private:
    shared_ptr<Dialogue> m_CashingDialogue = { nullptr };
    _uint m_CurIndex = { 0 };
    _bool m_bFIn = false;


public:
    static shared_ptr<CUI_NPC> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END
