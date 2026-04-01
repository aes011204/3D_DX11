#pragma once
#include "GameObject.h"
#include "Client_Define.h"
NS_BEGIN(Engine)

class CModel;
class CCollider;
NS_END


NS_BEGIN(Client)
class CMonster_Anim :
    public CGameObject
{
private:
    CMonster_Anim(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMonster_Anim(const CMonster_Anim& prototype);
public:
    virtual ~CMonster_Anim() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();

protected:
    HRESULT Ready_Components();
private:


private:

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CModel> m_pModelCom = { nullptr }; 
    shared_ptr<CTexture> m_pTextureCom = { nullptr };
    shared_ptr<CCollider> m_pColliderCom = { nullptr };
public:
    static shared_ptr<CMonster_Anim> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END