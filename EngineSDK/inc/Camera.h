#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera :
    public CGameObject
{
public:
    struct CAMERA_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float4 vEyes, vAt = {};
        _float fFovY, fNear, fFar = {};

    };
protected:
    CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCamera(const CCamera& prototype);
public:
    virtual ~CCamera() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

 protected:
     _float						m_fFovy{}, m_fNear{}, m_fFar{}, m_fAspect{};
 protected:
     void Update_TransformMatrices();

 public:
     //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
     static shared_ptr<CCamera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
     virtual shared_ptr<CGameObject> Clone(void* pArg) override;
     void Free() override;
};



NS_END
