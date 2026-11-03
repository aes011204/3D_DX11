#pragma once
#include "GameObject.h"

#include "PartObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CContainerObject :
    public CGameObject

{
public:
    struct ContainerObjDesc : public CGameObject::GameObjectDesc
    {
      
    };
protected:
    CContainerObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CContainerObject(const CContainerObject& prototype);
public:
    virtual ~CContainerObject() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* arg);
    virtual void Priority_Update(_float timeDelta);
    virtual void Update(_float timeDelta);
    virtual void Late_Update(_float timeDelta);
    virtual HRESULT Render();

protected:

    map<const _wstring, shared_ptr<class CPartObject>> m_partObjects;

protected:
    HRESULT Add_PartObject(_uint prototypeLevelIndex, const _wstring& prototypeTag,
        const _wstring& partTag, void* arg);
    shared_ptr<class CPartObject> Find_PartObject(const _wstring& partTag);
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CGameObject> Clone(void* arg) = 0;
    void Free() override;
};



NS_END
