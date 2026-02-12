#pragma once
#include "Base.h"
#include "Client_Define.h"
#include "Client_Enum.h"

NS_BEGIN(Client)

class CInventoryUI :
    public CBase ///일단 임시로  base 
{
private:
    CInventoryUI();
public:
    virtual ~CInventoryUI() = default;

public:
    HRESULT Initialize();
    void Update();

    bool MousePosToSlot(_uint& returnSlotX, _uint& returnSlotY);
private:


    class shared_ptr<class CInventory_Controller> m_Inven_Ctrl = { nullptr };


public:
    static shared_ptr<CInventoryUI> Create();
    virtual void Free() override;
};

NS_END