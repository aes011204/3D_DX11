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
    struct STORAGE_DESC : public CUIPanel::UIPANEL_DESC
    {
        INVENTYPE Inventype = { INVENTYPE::NONE };
        SHOPTAB shopTab = { SHOPTAB::NONE };
    };
private:
    CUI_Storage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Storage(const CUIPanel& prototype);
    HRESULT Initialize_Prototype();
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
    _bool MousePosToSlot();
    _float2 Calculate_RenderPos(const Item_Inst& item);
    void Render_Item();

    void Set_SlotNum(_uint col, _uint row);

    shared_ptr<CUIPanel> Get_InvenPanel() { return m_InvenPanel; }

private:
    INVENTYPE m_inventype = { INVENTYPE::NONE };
    SHOPTAB m_shopTab = { SHOPTAB::NONE };

    shared_ptr<class CInventory_Controller> Inven_Contrl = { nullptr };
    shared_ptr<CUIPanel> m_InvenPanel = { nullptr };

    weak_ptr<class CInventory> m_Inven = {};

    vector<shared_ptr<CUIImage>> m_ItemUI = {};
    vector<shared_ptr<CUISlot>> m_Slot = {};
public:
    static shared_ptr<CUI_Storage> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
    _bool Rebuild_InventorySlot(_uint w, _uint h, const vector<Slot>& inven_slot);
};

NS_END;