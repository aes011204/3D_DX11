#pragma once
#include "GameObject.h"


NS_BEGIN(Engine)

class ENGINE_DLL CPartObject abstract:
    public CGameObject

{
public:
    struct PartObjDesc : public CGameObject::GameObjectDesc
    {
        const _float4x4* parentMatrix = {nullptr};
    };
protected:
    CPartObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CPartObject(const CPartObject& prototype);
public:
    virtual ~CPartObject() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* arg);
    virtual void Priority_Update(_float timeDelta);
    virtual void Update(_float timeDelta);
    virtual void Late_Update(_float timeDelta);
    virtual HRESULT Render();

protected:
    const _float4x4* m_parentMatrix = { nullptr };
    _float4x4 m_combinedWorldMatrix = {};

protected:
   void Update_CombinedWorldMatirix(_fmatrix childrenMatrix)
   {
       XMStoreFloat4x4(&m_combinedWorldMatrix, childrenMatrix * XMLoadFloat4x4(m_parentMatrix));
   }
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CGameObject> Clone(void* arg)=0;
    void Free() override;
};



NS_END
