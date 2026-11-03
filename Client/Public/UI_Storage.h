#pragma once
#include "UIPanel.h"
#include "Client_Define.h"

namespace Engine
{
	class CUISlot;
	class CUIImage;
}

NS_BEGIN(Client)

	class CUI_Storage :
    public CUIPanel
{
public:
    struct StorageDesc : public CUIPanel::UIPanelDesc
    {
        EInvenType invenType = { EInvenType::NONE };
        EShopTab shopTab = { EShopTab::NONE };
    };
private:
    CUI_Storage(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CUI_Storage(const CUIPanel& prototype);
    HRESULT Initialize_Prototype();
public:
    ~CUI_Storage() = default;

public:



    HRESULT OnInit(void* arg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    void OnPrepareRender() override;
    HRESULT OnRender()override;
    void OnClear()override;
    _bool MousePosToSlot();
    _float2 Calculate_RenderPos(const ItemInst& item);
    void Render_Item();

    void Set_SlotNum(_uint col, _uint row);

    shared_ptr<CUIPanel> Get_InvenPanel() { return m_invenPanel; }

private:
    EInvenType m_inventype = { EInvenType::NONE };
    EShopTab m_shopTab = { EShopTab::NONE };

    shared_ptr<class CInventory_Controller> m_invenContr = { nullptr };
    shared_ptr<CUIPanel> m_invenPanel = { nullptr };

    weak_ptr<class CInventory> m_inven = {};

    vector<shared_ptr<CUIImage>> m_itemUI = {};
    vector<shared_ptr<CUISlot>> m_slot = {};
public:
    static shared_ptr<CUI_Storage> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    void Free() override;
    _bool Rebuild_InventorySlot(_uint w, _uint h, const vector<Slot>& invenSlot);
};

NS_END;