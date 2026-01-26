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
    virtual ~CInventoryUI() = default;

public:
    HRESULT Initialize();
    void Update();

    bool MousePosToSlot(_uint& returnSlotX, _uint& returnSlotY);
private:


    class CInventory_Controller* m_Inven_Ctrl = { nullptr };


public:
    static CInventoryUI* Create();
    virtual void Free() override;
};

NS_END