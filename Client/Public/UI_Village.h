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
    enum EVillageLocation {FISH, REPAIR, STORAGE,BOAT,END};
    struct VillageDesc : public CUIPanel::UIPanelDesc
    {
       /* _uint TextureComLevel = {};
        _wstring TextureProtoName = L"";*/

        //_bool IsTransparent = {};
       //_bool IsFullScreen = {};
       //_bool IsUseLayout = {};
    };
protected:
    CUI_Village(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CUI_Village(const CUIPanel& prototype);
public:
    ~CUI_Village() = default;

public:

    HRESULT OnInit(void* arg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

    virtual void UI_Active()override;

public:
    shared_ptr<CUIImage> m_select[3];
    shared_ptr<CUIImage> m_selectBoat[3];
private:

public:
    static shared_ptr<CUI_Village> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    void Free() override;
};

NS_END