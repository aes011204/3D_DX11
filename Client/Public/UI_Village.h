#pragma once
#include "UIPanel.h"
#include "Client_Define.h"

namespace Engine
{
	class CUIImage;
}

NS_BEGIN(Client)

	class CUI_Village final:
    public CUIPanel
{
public:
    enum VIllAGE_LOCATION {FISH, REPAIR, STORAGE,BOAT,END};
    struct MAINMENU_DESC : public CUIPanel::UIPANEL_DESC
    {
       /* _uint TextureComLevel = {};
        _wstring TextureProtoName = L"";*/

        //_bool IsTransparent = {};
       //_bool IsFullScreen = {};
       //_bool IsUseLayout = {};
    };
protected:
    CUI_Village(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Village(const CUIPanel& prototype);
public:
    ~CUI_Village() = default;

public:

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

    virtual void UI_Active()override;

public:
    shared_ptr<CUIImage> m_Select[3];
    shared_ptr<CUIImage> m_Select_boat[3];
private:

public:
    static shared_ptr<CUI_Village> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END