#pragma once
#include "GameObject.h"
#include "Client_Define.h"

namespace Engine
{
	class CCollider;
}

NS_BEGIN(Client)

	class CMonster_Tantacle :
    public CGameObject
{
public:
    enum STATE{ATTACK, IDLE, RELEASE, END};

private:
    CMonster_Tantacle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMonster_Tantacle(const CMonster_Tantacle& prototype);
public:
    virtual ~CMonster_Tantacle() = default;

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
    const _float4x4* m_pSocketMatrix_AttackPoint = { nullptr };


    XMMATRIX CombinedWorldMatrix(_fmatrix ChildrenMatrix)
    {
        return ChildrenMatrix * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix());
    }

private:

    _uint m_AnimIndex = {};
    STATE m_State = {};
private:

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CModel> m_pModelCom = { nullptr }; 
    shared_ptr<CTexture> m_pTextureCom = { nullptr };
    shared_ptr<CCollider> m_pColliderCom = { nullptr };

    shared_ptr<CCollider> m_Attack_Col = { nullptr };

public:

    static shared_ptr<CMonster_Tantacle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END