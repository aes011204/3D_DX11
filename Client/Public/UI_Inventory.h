#pragma once
#include "UIPanel.h"
#include "Client_Define.h"


// Inven_controller -> UIInventory // 변경하면 안됨 // 컨트롤러가 이밴트 이용하여 신호 -> 나 모양 바뀜
// UIInventory -> Inven_controller 알아도 됨 , 매니져나 초기 생성할떄 포인터 넘겨 주기


NS_BEGIN(Engine)
class CUIImage;
class CUISlot;
NS_END

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
    HRESULT Initialize_Prototype() override;

    _bool Rebuild_InventorySlot(_uint w, _uint h, const vector<Slot>& inven_slot);


    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;
 
    void Render_Item();
    _float2 SlotToPos(int col, int row);
    _float2 Calculate_RenderPos(const Item_Inst& item);


private:
    weak_ptr<class CInventory> m_Inven = {};


    shared_ptr<CUIPanel> m_InvenPanel = { nullptr };
    vector<shared_ptr<CUIImage>> m_ItemUI = {};
    vector<shared_ptr<CUISlot>> m_Slot = {};
public:
    static shared_ptr<CUI_Inventory> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END