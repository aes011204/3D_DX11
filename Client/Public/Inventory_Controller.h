#pragma once
#include "Base.h"
#include "Client_Define.h"
#include "Client_Enum.h"
#include "Inventory.h"

// 싱글톤 or 전역 마우스커서와 비슷하게사용
NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
	class CInventory;

class CInventory_Controller :
    public CBase
{
    DECLARE_SINGLETON(CInventory_Controller)

private:
    CInventory_Controller();
public:
    virtual ~CInventory_Controller() = default;

public:
    HRESULT Initialize();
    void Update(float TimeDelta);

    void Set_Inven(weak_ptr<class CInventory> inventory) { m_Inven = inventory; }


    _bool PickUp_Item(Item_Inst HoldItem) {
        m_HoldItem = HoldItem; Is_Dragging = true;
        m_Inven.lock()->RemoveFrom_Inven(m_HoldItem.ItemInst_ID);
    }

   //const vector<Slot>& Get_Invanslot(_uint& w, _uint& h) const{
   //      w = m_Inven->Get_W(); h = m_Inven->Get_H(); return m_Inven->Get_Invenslot();
   // }

    //HRESULT Create_HoldItem(Item_Inst HoldItem) { m_HoldItem = HoldItem};
private:
    Item_Inst m_HoldItem = {};
    vector<_float2> m_OffSet = {};
    /*_float m_Rotaion = {};*/ // 이거 인스턴스 안에 있음
    bool Is_Dragging = false;

    class weak_ptr<class CInventory> m_Inven = {};
    class shared_ptr<class CInventoryUI> m_UIInven = { nullptr };
    class weak_ptr<CGameInstance>m_pGameInstance = { };
public:
    //static shared_ptr<CInventory_Controller> Create();
    //virtual void Free() override;
};

NS_END