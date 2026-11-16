#pragma once


#include "UIPanel.h"
#include "Client_Define.h"
NS_BEGIN(Client)
class CUI_HUD final :
    public CUIPanel
{
public:
    struct HUDDesc : public CUIPanel::UIPanelDesc
    {
    };
protected:
    CUI_HUD(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CUI_HUD(const CUIPanel& prototype);
public:
    ~CUI_HUD() = default;

public:

    HRESULT OnInit(void* arg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

public:

private:

public:
    static shared_ptr<CUI_HUD> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    void Free() override;
};

NS_END