#pragma once
#include "UIPanel.h"
#include "Client_Define.h"
//오른쪽에서 나오는 효과있음
NS_BEGIN(Client)
class CUI_TabContainer :
    public CUIPanel
{
public:
    struct TABCONTAINER_DESC : public CUIPanel::UIPANEL_DESC
    {
    };
protected:
    CUI_TabContainer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_TabContainer(const CUIPanel& prototype);
public:
    ~CUI_TabContainer() = default;

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
    static shared_ptr<CUI_TabContainer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END