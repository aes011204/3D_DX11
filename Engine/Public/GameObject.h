#pragma once
#include "Transform.h" // 이 안에 베이스 포함되있음
#include "Entity.h"

NS_BEGIN(Engine)
class CCollider;
class ENGINE_DLL CGameObject abstract : public CEntity
{
public:
    struct GameObjectDesc : public CTransform::TransformDesc
    {
        _uint flag = {};

        //_float fX = {};
        //_float fY = {};
        //_float fSizeX = {};
        //_float fSizeY = {};

    };
protected:
    CGameObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CGameObject(const CGameObject& prototype);
public:
    virtual ~CGameObject() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* arg);
    virtual void Priority_Update(_float timeDelta);
    virtual void Update(_float timeDelta);
    virtual void Late_Update(_float timeDelta);
    virtual HRESULT Render();

public:
    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) {};
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other){}
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) {};


    shared_ptr<CTransform> Get_TransformCom() {return m_transformCom;}
protected:
    
    class shared_ptr<CTransform> m_transformCom = { nullptr };

public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CGameObject> Clone(void* arg) = 0 ;
    void Free() override;
};



NS_END
