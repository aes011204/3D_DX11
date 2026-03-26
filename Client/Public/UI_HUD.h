#pragma once


#include "UIPanel.h"
#include "Client_Define.h"
NS_BEGIN(Client)
class CUI_HUD final :
    public CUIPanel
{
public:
    struct HUD_DESC : public CUIPanel::UIPANEL_DESC
    {
    };
protected:
    CUI_HUD(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_HUD(const CUIPanel& prototype);
public:
    ~CUI_HUD() = default;

public:

    HRESULT OnInit(void* pArg) override;
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
    static shared_ptr<CUI_HUD> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END