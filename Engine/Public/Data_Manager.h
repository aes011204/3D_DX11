#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CData_Manager :
    public CBase
{
    //DECLARE_SINGLETON(CData_Manager)

private:
    CData_Manager();
public:
    virtual ~CData_Manager() ;

public:
    HRESULT Initialize(_uint  EditorLevel); // 생성 할떄 호출
public:

    bool Reload(const _tchar* Path);// 이건그냥 임구이에서 하면 될듯
    bool ClearMap(SAVETYPE eDATATYPE);
    bool Load(SAVETYPE eDATATYPE, const string& fileName );
    bool Save(SAVETYPE eDATATYPE, const string& fileName);

public:
    HRESULT Load_ObjData(const string& fileName);
    HRESULT Save_ObjData(const string& fileName);

    HRESULT Load_UIData(const string& fileName);
    HRESULT Save_UIData(const string& fileName);

private:
    const _tchar* CurLoadedMapPath = {};
    weak_ptr<class CGameInstance> m_pGameInstance = {};
    _uint  m_EditorLevel = {};
public:
    static unique_ptr<CData_Manager> Create(_uint  EditorLevel);
    void Free() override;
};

NS_END