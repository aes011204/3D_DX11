#pragma once
#include "Client_Define.h"
#include "Client_Enum.h"
#include "Component.h"


NS_BEGIN(Client)

class CInventory :
    public CComponent
{
public:
    struct INVEN_DESC
    {
        _uint width = {};
        _uint height = {};
        INVENTYPE invenType = {};
        //vector<_char> type = {};
    };

    struct BoatLevelData
    {
        _uint m_width = {};
        _uint m_height = {};
        vector<_char> m_type = {};
    };
private:
    CInventory(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
    virtual ~CInventory() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    Item_Inst Create_ItemInstance(ID_uint itemDefID, int rot);

    void Update(_float fTimeDelta);

    vector<Slot>& Get_Invenslot() { return m_InvenSlot; }
    vector<Item_Inst>& Get_InventoryItem() { return m_Inventory; }
    int Get_W() { return m_w; }
    int Get_H() { return m_h; }
private:
    int m_w, m_h = {0};
    vector<Item_Inst> m_Inventory = {}; // 순서 상관없이 들어있는데이터
	vector<Slot> m_InvenSlot = {}; // 칸별로 점유중인지 아닌지

    class shared_ptr<class CInventory_Controller> m_Controller;
    // 컨트롤러는 싱글톤? 아님 여기서 만들어야 하나?

public:// 아이템 넣고 뺴기
    Item_Inst AddItem(Item_Inst itemInst, _int BaseX, _int BaseY);
    _int CanPlace(Item_Inst& itemInst, _uint BaseX, _uint BaseY,PLACE_COLOR& color);
    Item_Inst TryMove_Item(_uint BaseX, _uint BaseY);
    void Upgrade_Boat(_uint index);

    void ThrowAwayFrom_Inven(_uint BaseX, _uint BaseY);// 이거는 칸으로 
    Item_Inst RemoveFrom_Inven(int inst_id);// 이거는 아이디로

    void OnGui() override;
private:
    void PlaceOn_Inven(Item_Inst itemInst, _int BaseX, _int BaseY);

private:// 인밴칸 설정
    static constexpr int m_MaxBoatLevel = 3;
    BoatLevelData m_BoatUpgrade_type[m_MaxBoatLevel] = {};

private:
    void Apply_BaseMask(vector<Slot>& vecSlot);// 락
    SLOT_TYPE CharToType(char c);// 슬랏 타입
    void Init_BoatUpgrade();
    

public:
    static shared_ptr<CInventory> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<CComponent> Clone (void* pArg) override;
	void Free() override;
};

NS_END
