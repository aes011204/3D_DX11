#pragma once
#include "Client_Define.h"
#include "Base.h"

#include "Client_Enum.h"

NS_BEGIN(Client)

class CData_Manager :
    public CBase
{
    DECLARE_SINGLETON(CData_Manager)

private:
    CData_Manager();
    virtual ~CData_Manager() = default;

private:
    HRESULT Initialize(); // 생성 할떄 호출


private:
    HRESULT Load_ItemDate(const string& fileName);
    wstring Utf8ToWstring(const string& str);


private:// for Item_def
    void Parse_Fish_Def(Fish_Def& fish_def, const nlohmann::json& node);
    void Parse_Equip_Def(Equip_Def& fish_def, const nlohmann::json& node);

    SEA_MASK BitFlag_SeaType (const nlohmann::json& node,const string str);

private:
    vector<Item_Def>               m_vec_item_Defs;
public:
    CData_Manager* Create();
    void Free() override;
};



NS_END
