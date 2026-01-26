#pragma once
#include "Client_Define.h"
#include "Base.h"

#include "Client_Enum.h"

// json 파일든 정적데이터 로드 , 조회

NS_BEGIN(Client)

class CData_Manager :
    public CBase
{
    DECLARE_SINGLETON(CData_Manager)

private:
    CData_Manager();
    virtual ~CData_Manager() = default;

public:
    HRESULT Initialize(); // 생성 할떄 호출
    const Item_Def& GetItemByID(_uint ItemID); 

    _wstring Utf8ToWstring(const string& str);
private:
    _uint GetIndexByID(_uint id); // 바로 접근 하기 위한 벡터의 인덱스를 반환한다 

private:// for Item_def
    HRESULT Load_ItemDate(const string& fileName);
private:
    void Parse_Fish_Def(Fish_Def& fish_def, const nlohmann::json& node);
    void Parse_Equip_Def(Equip_Def& fish_def, const nlohmann::json& node);

    SEA_MASK BitFlag_SeaType (const nlohmann::json& node,const string str);
    void Compute_Occ(Shape& shape, _uint w, _uint h);
private:
    vector<Item_Def>               m_vec_ItemDefs;
    unordered_map<_uint, _uint> m_map_ItemID;
public:
    CData_Manager* Create();
    void Free() override;
};



NS_END
