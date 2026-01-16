#pragma once
#include "Client_Define.h"
#include "Base.h"

#include "Client_Struct.h"

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

private:
    vector<Item>               m_vecItems;
public:
    CData_Manager* Create();
    void Free() override;
};

NS_END