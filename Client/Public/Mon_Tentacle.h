#pragma once
#include "GameObject.h"
#include "Client_Define.h"

namespace Engine
{
	class CCollider;
}

NS_BEGIN(Client)

	class CMon_Tentacle :
    public CGameObject
{
public:
    enum STATE{ATTACK, IDLE, RELEASE, END};

private:
    CMon_Tentacle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMon_Tentacle(const CMon_Tentacle& prototype);
public:
    virtual ~CMon_Tentacle() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
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
    const _float4x4* m_pSocketMatrix_Top = { nullptr };
    const _float4x4* m_pSocketMatrix_Mid = { nullptr };
    const _float4x4* m_pSocketMatrix_Btm = { nullptr };
    const _float4x4* m_pSocketMatrix_Btm_F = { nullptr };

    XMMATRIX CombinedWorldMatrix(_fmatrix ChildrenMatrix)
    {
        return ChildrenMatrix * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix());
    }

private:

    float m_TurnSpeed = {};
    _bool m_IsColl = { false };

    _uint m_AnimIndex = {};
    STATE m_State = {};
    _bool m_Flag = false ;

    _vector m_LastDir = {};

private:

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CModel> m_pModelCom = { nullptr }; 
    shared_ptr<CTexture> m_pTextureCom = { nullptr };
    //shared_ptr<CCollider> m_pColliderCom = { nullptr };

    shared_ptr<CCollider> m_Top_Collider = { nullptr };
    shared_ptr<CCollider> m_Mid_Collider = { nullptr };
    shared_ptr<CCollider> m_Btm_Collider = { nullptr };
    shared_ptr<CCollider> m_Btm_F_Collider = { nullptr };

    weak_ptr<CGameObject> m_pPlayer = {  };
public:

    static shared_ptr<CMon_Tentacle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END