#pragma once
#include "Client_Define.h"
#include "Base.h"

#include "Client_Enum.h"

// json 파일든 정적데이터 로드 , 조회


NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Client)

class CItemDB :
    public CBase
{
    DECLARE_SINGLETON(CItemDB)

private:
    CItemDB();
public:
    virtual ~CItemDB() = default;

public:
    HRESULT Initialize(); // 생성 할떄 호출
    const ItemDef& GetItemByID(_uint itemId); 

    _wstring Utf8ToWstring(const string& str);
private:
    _uint GetIndexByID(_uint id); // 바로 접근 하기 위한 벡터의 인덱스를 반환한다 

private:// for Item_def
    HRESULT Load_ItemData(const string& fileName);
private:
    void Parse_Fish_Def(FishDef& fishDef, const nlohmann::json& node);
    void Parse_Equip_Def(EquipDef& fishDef, const nlohmann::json& node);

    SeaMask BitFlag_SeaType (const nlohmann::json& node,const string str);
    void Compute_Occ(Shape& shape, _uint w, _uint h);
private:
    vector<ItemDef>               m_vecItemDefs;
    unordered_map<_uint, _uint> m_mapItemId;

    weak_ptr<CGameInstance >m_gameInstance = {};
public:
    //shared_ptr<CData_Manager> Create();
    void Free() override;
};



NS_END
