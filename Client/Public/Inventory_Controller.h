#pragma once
#include "GameObject.h"
#include "Client_Define.h"
#include "Client_Enum.h"
#include "Inventory.h"

// 싱글톤 or 전역 마우스커서와 비슷하게사용
// 게임 오브젝트 인데 1개만 만들거임
NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
	class CInventory;

class CInventory_Controller :
    public CGameObject
{
    //DECLARE_SINGLETON(CInventory_Controller)
    CInventory_Controller(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
    virtual ~CInventory_Controller() = default;

public:
    HRESULT Initialize(weak_ptr<CInventory> Inven, shared_ptr<class CUI_Item> UIHoldItem);
    void Update(float TimeDelta);
    void Make_Hold(Item_Inst inst);
    shared_ptr<CInventory> Get_CurrentInven();

    void Set_Inven(weak_ptr<class CInventory> inventory) { m_PlayerInven = inventory; }

    bool Is_Dragging() { return m_bDragging; }
    //_bool PickUp_Item(Item_Inst HoldItem) {
    //    m_HoldItem = HoldItem; m_bDragging = true;
    //    m_Inven.lock()->RemoveFrom_Inven(m_HoldItem.ItemInst_ID);
    //}

   //const vector<Slot>& Get_Invanslot(_uint& w, _uint& h) const{
   //      w = m_Inven->Get_W(); h = m_Inven->Get_H(); return m_Inven->Get_Invenslot();
   // }

    //HRESULT Create_HoldItem(Item_Inst HoldItem) { m_HoldItem = HoldItem};
private:
    //Item_Inst m_HoldItem = {};
    vector<_float2> m_OffSet = {};
    /*_float m_Rotaion = {};*/ // 이거 인스턴스 안에 있음

    _uint m_prevItemInstId = {};//툴팁용 체크
    //bool prevMouseOnSlot = { false };
    bool m_bDragging = false;
    bool m_PrevDragging = false;
    _uint m_SlotX, m_SlotY = {};
    _uint m_PrevSlotX, m_PrevSlotY = {};
    _bool m_bIsOnSlot = { false };


    _bool m_bPrevOnSlot = { false };

    // 상점 등등을 위한 인밴 데이터도 필요함
     weak_ptr<class CInventory> m_PlayerInven = {}; // 로직
    weak_ptr<CInventory> m_TargetInven;


    weak_ptr<CInventory> m_SourceInven;
    weak_ptr<CInventory> m_DestInven;

    _bool m_IsPlayerSlot = false;

   // class shared_ptr<class CUI_Inventory> m_UIInven = { nullptr }; // UI
   shared_ptr<class CUI_Item> m_UIHoldItem = { nullptr };
   weak_ptr<CGameInstance>m_pGameInstance = { };
   INVENTYPE m_HoldItemFrom;


public:
    static shared_ptr<CInventory_Controller> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, weak_ptr<CInventory> Inven,shared_ptr<CUI_Item> UIHoldItem);
    virtual void Free() override;

    shared_ptr<CGameObject> Clone(void* pArg) { return shared_ptr<CGameObject>{}; };
};



NS_END
