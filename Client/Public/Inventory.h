#pragma once
#include "Client_Define.h"
#include "Client_Enum.h"
#include "Base.h"


NS_BEGIN(Client)

class CInventory :
    public CBase
{
private:
    CInventory();
public:
    virtual ~CInventory() = default;

public:
    HRESULT Initialize();
    void Update(_float fTimeDelta);



private:
    int w, h = {0};
    vector<Item_Inst> m_Inventory = {};
	vector<Slot> m_InvenSlot = {};

    class shared_ptr<class CInventory_Controller> m_Controller;
    // 컨트롤러는 싱글톤? 아님 여기서 만들어야 하나?

public:// 아이템 넣고 뺴기
    Item_Inst AddItem(Item_Inst itemInst, _int BaseX, _int BaseY);
    _int CanPlace(Item_Inst& itemInst, _uint BaseX, _uint BaseY,PLACE_COLOR& color);
    Item_Inst TryMove_Item(_uint BaseX, _uint BaseY);
    void ThrowAwayFrom_Inven(_uint BaseX, _uint BaseY);
private:
    Item_Inst RemoveFrom_Inven(int inst_id);
    void PlaceOn_Inven(Item_Inst itemInst, _int BaseX, _int BaseY);

private:// 인밴칸 설정
    static constexpr int m_MaxBoatLevel = 3;
    vector<_char> m_BoatUpgrade_type[m_MaxBoatLevel] = {};

private:
    void Apply_BaseMask(vector<Slot>& vecSlot);// 락
    SLOT_TYPE CharToType(char c);// 슬랏 타입
    void Init_BoatUpgrade();
    void Upgrade_Boat(vector<_char> upgrade);

public:
    static shared_ptr<CInventory> Create(); 
    void Free() override;
};

NS_END
