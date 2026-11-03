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
    CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPartObject(const CPartObject& prototype);
public:
    virtual ~CPartObject() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

protected:
    const _float4x4* m_parentMatrix = { nullptr };
    _float4x4 m_combinedWorldMatrix = {};

protected:
   void Update_CombinedWorldMatirix(_fmatrix ChildrenMatrix)
   {
       XMStoreFloat4x4(&m_combinedWorldMatrix, ChildrenMatrix * XMLoadFloat4x4(m_parentMatrix));
   }
public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CGameObject> Clone(void* pArg)=0;
    void Free() override;
};



NS_END
