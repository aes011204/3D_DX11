#pragma once
#include "Client_Define.h"
#include "Client_Enum.h"
#include "Component.h"


NS_BEGIN(Client)

class CInventory :
    public CComponent
{
public:
    struct InvenDesc
    {
        _uint width = {};
        _uint height = {};
        EInvenType invenType = {};
        EEquipType equipType = {};
        //vector<_char> type = {};
    };

    struct BoatLevelData
    {
        _uint width = {};
        _uint height = {};
        vector<_char> type = {};
    };
private:
    CInventory(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
    virtual ~CInventory() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* arg);
    ItemInst Create_ItemInstance(IdType itemDefId, variant<monostate, FishInst, EquipInst>  typeDefInst, int rot=0);

    void Update(_float timeDelta);

    vector<Slot>& Get_Invenslot() { return m_invenSlot; }
    vector<ItemInst>& Get_InventoryItem() { return m_inventory; }
    int Get_W() { return m_w; }
    int Get_H() { return m_h; }

    EInvenType  Get_Inventype() { return m_invenType; }
private:
    int m_w, m_h = {0};
    vector<ItemInst> m_inventory = {}; // 순서 상관없이 들어있는데이터
	vector<Slot> m_invenSlot = {}; // 칸별로 점유중인지 아닌지

    _bool m_isDirty = { false };

    class shared_ptr<class CInventory_Controller> m_controller;
    // 컨트롤러는 싱글톤? 아님 여기서 만들어야 하나?

public:// 아이템 넣고 뺴기
    ItemInst AddItem(ItemInst itemInst, _int baseX, _int baseY);
    _bool Auto_Add(ItemInst& itemInst);
    _bool Auto_Move_To(weak_ptr<CInventory> otherInven, _uint baseX, _uint baseY);
    _int CanPlace(ItemInst& itemInst, _uint baseX, _uint baseY,EPlaceColor& color);
    ItemInst TryMove_Item(_uint baseX, _uint baseY);
    void Upgrade_Boat(_uint index);
    void SetHighlightArea(ItemInst& itemInst, _uint baseX, _uint baseY, EPlaceColor color);
    void FixAll();
    void ThrowAwayFrom_Inven(_uint baseX, _uint baseY);// 이거는 칸으로 
    void ClearHighlightArea();
    ItemInst RemoveFrom_Inven(int instId);// 이거는 아이디로
    ItemInst Peek_Item(_uint musX, _uint musY);




    void OnGui() override;
    Evt_ShipStat CalculateEquip();// 장비 인포 배에 업뎃
    void Get_Damage();// 장비 인포 배에 업뎃


    _bool Get_Dirty() const { return m_isDirty; }
    void Set_Dirty(_bool dirty) { m_isDirty = dirty; }
private:
    void PlaceOn_Inven(ItemInst itemInst, _int baseX, _int baseY);

private:// 인밴칸 설정
    static constexpr int kMaxBoatLevel = 3;
    BoatLevelData m_boatUpgradeType[kMaxBoatLevel] = {};

private:
    void Apply_BaseMask(vector<Slot>& vecSlot);// 락
    ESlotType CharToType(char c);// 슬랏 타입
    void Init_BoatUpgrade();

    EInvenType m_invenType = {};
    

public:
    static shared_ptr<CInventory> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

    virtual shared_ptr<CComponent> Clone (void* arg) override;
	void Free() override;
};

NS_END
