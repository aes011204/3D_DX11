#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CCamera_Manager :
    public CBase
{ 
private:
    CCamera_Manager();
public:
    virtual ~CCamera_Manager();

 
public:

    HRESULT Initialize(_uint iNumLevels);
    void Apply_To_PipeLine();
   
public:

public:
    HRESULT Add_Camera(_uint camLevel, _wstring key, shared_ptr<class CCamera> cam);
   
    _bool Change_Camera(_wstring key);
    HRESULT Clear_Camera(_uint level);

   
    ///
    void OnGui()override;
protected:
    shared_ptr<class CCamera> Find_Camera(_wstring key);


private:
    _uint  m_iNumLevel = {};
    shared_ptr<class CCamera> m_CurCam = {};
    map<const _wstring, shared_ptr<class CCamera>>* m_Cameras = {};
  
private:

    weak_ptr<class CGameInstance> m_pGameInstance = {};
public:

    static unique_ptr<CCamera_Manager> Create(_uint iNumLevels);
    void Free() override;
};

NS_END