#pragma once
#include "UIPanel.h"
#include "Client_Define.h"

namespace Engine
{
	class CUIImage;
	class CUIText;
}

NS_BEGIN(Client)

	class CUI_MiniGame :
    public CUIPanel
{
public:
    enum MINIGAME {BASIC_CIRCLE, END};
    struct MINIGAEMEPANEL_DESC : public CUIPanel::UIPANEL_DESC
    {

    };
protected:
    CUI_MiniGame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_MiniGame(const CUIPanel& prototype);
public:
    ~CUI_MiniGame() = default;

public:

    void UI_PanelActive(MINIGAME MiniGameState, _uint Defid);

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

public:
    Zone m_zones[8];
    float m_zoneCount;
private:
    shared_ptr<CUIText> m_nameTex = { nullptr };
    shared_ptr<CUIText> m_amountTex = { nullptr };
    shared_ptr<CUIText> m_locationTex = { nullptr };
    shared_ptr<CUIImage> m_FishIcon = { nullptr };
    shared_ptr<CUIImage> m_pCircle = { nullptr };
    shared_ptr<CShader> m_CircleShader = { nullptr };

public:
    static shared_ptr<CUI_MiniGame> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END