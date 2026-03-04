#pragma once
#include "UIPanel.h"
#include "Client_Define.h"

NS_BEGIN(Client)
class CUI_Inventory :
    public CUIPanel
{
public:
    struct INVENTORY_DESC : public CUIPanel::UIPANEL_DESC
    {
       
    };
private:
    CUI_Inventory(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Inventory(const CUIPanel& prototype);
public:
    ~CUI_Inventory() = default;

public:

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;


private:

public:
    static shared_ptr<CUI_Inventory> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END