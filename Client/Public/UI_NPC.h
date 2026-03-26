#pragma once
#include "UIPanel.h"

#include "Client_Define.h"

namespace Engine
{
	class CUIImage;
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
    void UI_NPCActive(NPC npc, _bool dialogue, _bool NPCImg);
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
private:

public:
    static shared_ptr<CUI_NPC> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END
