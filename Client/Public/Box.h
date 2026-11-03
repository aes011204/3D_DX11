#pragma once
#include "GameObject.h"
#include "Client_Define.h"

namespace Engine
{
	class CCollider;
}

NS_BEGIN(Client)

	class CBox :
    public CGameObject
{
public:

    enum EState{ATTACK, IDLE, RELEASE, END};

private:
    CBox(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CBox(const CBox& prototype);
public:
    virtual ~CBox() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Priority_Update(_float timeDelta) override;
    virtual void Update(_float timeDelta) override;
    virtual void Late_Update(_float timeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;

    virtual void OnGui() override;

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();

protected:
    HRESULT Ready_Components();

    // 손에 콜라이더
    const _float4x4* m_socketMatrixRightHand = { nullptr };
    const _float4x4* m_socketMatrixLeftHand = { nullptr };

    XMMATRIX CombinedWorldMatrix(_fmatrix childrenMatrix)
    {
        return childrenMatrix * XMLoadFloat4x4(m_transformCom->Get_WorldMatrix());
    }

private:

    _uint m_animIndex = {};
    EState m_state = {};
private:

    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CModel> m_modelCom = { nullptr }; 
    shared_ptr<CTexture> m_textureCom = { nullptr };
    shared_ptr<CCollider> m_colliderCom = { nullptr };

    shared_ptr<CInventory> m_storageCom = { nullptr };

public:

    static shared_ptr<CBox> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;



};

NS_END