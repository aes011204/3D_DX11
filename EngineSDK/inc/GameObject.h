#pragma once
#include "Transform.h" // 이 안에 베이스 포함되있음
#include "Entity.h"

NS_BEGIN(Engine)
class CCollider;
class ENGINE_DLL CGameObject abstract : public CEntity
{
public:
    struct GAMEOBJECT_DESC : public CTransform::TRANSFOM_DESC
    {
        _uint iFlag = {};

        //_float fX = {};
        //_float fY = {};
        //_float fSizeX = {};
        //_float fSizeY = {};

    };
protected:
    CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CGameObject(const CGameObject& prototype);
public:
    virtual ~CGameObject() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) {};
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other){}
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) {};

protected:
    
    class shared_ptr<CTransform> m_pTransformCom = { nullptr };

public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CGameObject> Clone(void* pArg) = 0 ;
    void Free() override;
};



NS_END
