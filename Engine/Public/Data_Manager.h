#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CData_Manager :
    public CBase
{
    DECLARE_SINGLETON(CData_Manager)

private:
    CData_Manager();
public:
    virtual ~CData_Manager() = default;

public:
    HRESULT Initialize(); // 생성 할떄 호출


private:

    bool Reload(const _tchar* Path);
    bool ClearMap();
    bool Load();
    bool Save();

private:
    HRESULT Load_ObjData(const string& fileName, _uint  EditorLevel);
    HRESULT Save_ObjData(const string& fileName);

    HRESULT Load_UIData(const string& fileName);
    HRESULT Save_UIData(const string& fileName);

private:
    const _tchar* CurLoadedMapPath = {};
    weak_ptr<class CGameInstance> m_pGameInstance = {};

public:
    //shared_ptr<CData_Manager> Create();
    void Free() override;
};

NS_END