#pragma once
#include "Base.h"
#include "Client_Define.h"
#include "Client_Enum.h"

NS_BEGIN(Client)

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

private:
    Item_Inst m_HoldItem = {};
    vector<_float2> m_OffSet = {};
    bool Is_Dragging = false;

    class shared_ptr<class CInventory> m_Inven = { nullptr };
    class shared_ptr<class CInventoryUI> m_UIInven = { nullptr };

public:
    static shared_ptr<CInventory_Controller> Create();
    virtual void Free() override;
};

NS_END