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
    struct NpcDesc : public CUIPanel::UIPanelDesc
    {
    };
protected:
    CUI_NPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_NPC(const CUIPanel& prototype);
public:
    ~CUI_NPC() = default;

public:

    HRESULT OnInit(void* pArg) override;
    void UI_NPCActive(const string& dialogueId, list<string>& Fistlist);
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
    shared_ptr<CUIImage> m_dialogue = { nullptr };
    shared_ptr<CUIImage> m_npcImg = { nullptr };
    shared_ptr<CUIImage> m_nameBase = { nullptr };
    shared_ptr<CUIImage> m_backImg = { nullptr };
    shared_ptr<CUIText> m_nameText = { nullptr };
    shared_ptr<CUIText> m_dialogueText = { nullptr };

    shared_ptr<CUIButton>m_rightChoice = { nullptr };
    shared_ptr<CUIButton>m_leftChoice = { nullptr };
    shared_ptr<CUIText> m_upText = { nullptr };
    shared_ptr<CUIText> m_downText = { nullptr };
private:
    shared_ptr<Dialogue> m_cachedDialogue = { nullptr };
    _uint m_curIndex = { 0 };
    _bool m_isFinished = false;

    list<string> m_dialogueList;

public:
    static shared_ptr<CUI_NPC> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END
