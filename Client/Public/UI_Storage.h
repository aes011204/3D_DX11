#pragma once
#include "UIPanel.h"
#include "Client_Define.h"
NS_BEGIN(Client)
class CUI_Storage :
    public CUIPanel
{
public:
    struct STORAGE_DESC : public CUIPanel::UIPANEL_DESC
    {

    };
private:
    CUI_Storage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Storage(const CUIPanel& prototype);
public:
    ~CUI_Storage() = default;

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
    shared_ptr<class CInventory_Controller> Inven_Contrl = { nullptr };
    shared_ptr<CUIPanel> m_InvenPanel = { nullptr };
public:
    static shared_ptr<CUI_Storage> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;

};

NS_END;