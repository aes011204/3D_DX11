
#pragma once
#include "GameObject.h"
#include "Client_Define.h"

NS_BEGIN(Client)

class CColliderObject :
    public CGameObject
{
private:
    CColliderObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CColliderObject(const CColliderObject& prototype);
public:
    virtual ~CColliderObject() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;

    virtual void OnGui() override;
    virtual void RebindCom();

private:
    HRESULT Ready_Components();

private:
    shared_ptr<CCollider> m_pColliderCom = { nullptr };

public:
    static shared_ptr<CColliderObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;
};

NS_END
