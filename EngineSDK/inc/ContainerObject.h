#pragma once
#include "GameObject.h"

#include "PartObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CContainerObject :
    public CGameObject

{
public:
    struct ContainerOBJ_DESC : public CGameObject::GAMEOBJECT_DESC
    {
      
    };
protected:
    CContainerObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CContainerObject(const CContainerObject& prototype);
public:
    virtual ~CContainerObject() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

protected:

    map<const _wstring, shared_ptr<class CPartObject>> m_PartObjects;

protected:
    HRESULT Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
        const _wstring& strPartTag, void* pArg);
    shared_ptr<class CPartObject> Find_PartObject(const _wstring& strPartTag);
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CGameObject> Clone(void* pArg) = 0;
    void Free() override;
};



NS_END
