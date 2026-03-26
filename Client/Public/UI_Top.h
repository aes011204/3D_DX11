#pragma once


#include "UIPanel.h"
#include "Client_Define.h"
NS_BEGIN(Client)
class CUI_Top final :
    public CUIPanel
{
public:
    struct Top_DESC : public CUIPanel::UIPANEL_DESC
    {
    };
protected:
    CUI_Top(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Top(const CUIPanel& prototype);
public:
    ~CUI_Top() = default;

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
    static shared_ptr<CUI_Top> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END