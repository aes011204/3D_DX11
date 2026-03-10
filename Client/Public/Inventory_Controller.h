#pragma once
#include "Base.h"
#include "Client_Define.h"
#include "Client_Enum.h"
#include "Inventory.h"


NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
	class CInventory;

class CInventory_Controller :
    public CBase
{
private:
    CInventory_Controller();
public:
    virtual ~CInventory_Controller() = default;

public:
    HRESULT Initialize();
    void Update();


   const vector<Slot>& Get_Invanslot(_uint& w, _uint& h) const{
         w = m_Inven->Get_W(); h = m_Inven->Get_H(); return m_Inven->Get_Invenslot();
    }

private:
    Item_Inst m_HoldItem = {};
    vector<_float2> m_OffSet = {};
    bool Is_Dragging = false;

    class shared_ptr<class CInventory> m_Inven = { nullptr };
    class shared_ptr<class CInventoryUI> m_UIInven = { nullptr };
    class weak_ptr<CGameInstance>m_pGameInstance = { };
public:
    static shared_ptr<CInventory_Controller> Create();
    virtual void Free() override;
};

NS_END