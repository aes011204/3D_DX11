#pragma once
#include "GameObject.h"
#include "Client_Define.h"

namespace Engine
{
	class CCollider;
}

NS_BEGIN(Client)

	class CMon_MonkFish :
    public CGameObject
{
public:
    enum EState{ATTACK, IDLE, P_NEAR,REVEAL, RUNAWAY, END};
    enum class EDistState { ATTACK_RANGE, E_NEAR, E_FAR, END };

private:
    CMon_MonkFish(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMon_MonkFish(const CMon_MonkFish& prototype);
public:
    virtual ~CMon_MonkFish() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    void ChangeState(EState newState);
    void EnterState(EState newState);
    EDistState GetDistanceState(_float dist) const;
    void UpdateBlendByDistanceState(EDistState distState, _float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;

    virtual void OnGui() override;

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();
    HRESULT Bind_ShaderResources_Mesh();
    virtual _vector Get_WorldPos() ;
   
protected:
    HRESULT Ready_Components();

  
    const _float4x4* m_socketMatrix = { nullptr };
    const _float4x4* m_socketMatrixLight = { nullptr };
    //const _float4x4* m_pSocketMatrix_LefttHand = { nullptr };

    XMMATRIX CombinedWorldMatrix(_fmatrix ChildrenMatrix)
    {
        return ChildrenMatrix * XMLoadFloat4x4(m_transformCom->Get_WorldMatrix());
    }

private:
    _int m_attackPrevFrame = {};
    _uint m_animIndex = {};
    EState m_state = {};
    EState m_prevState = {};

    _float m_alphaMesh = {};
    _float m_alphaAnim = {};
    _float m_alphaSpeed = {};
    _float3 m_dir = {};


    _bool IsColl = {false};
private:

    shared_ptr<CShader> m_shaderCom = { nullptr };
     shared_ptr<CShader> m_shaderComMesh = { nullptr };
     shared_ptr<CModel> m_modelComBoat = { nullptr };
    shared_ptr<CModel> m_modelComMon = { nullptr };
    shared_ptr<CTexture> m_textureCom = { nullptr };
    shared_ptr<CCollider> m_colliderCom = { nullptr };
    weak_ptr<CGameObject>  m_player = {  };


    shared_ptr<CLight> m_lightObj = { nullptr };

    _float m_lengthNear = {};
    _float m_lengthAttack = {};

public:

    static shared_ptr<CMon_MonkFish> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END
