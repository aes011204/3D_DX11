#pragma once

#pragma once
#include "GameObject.h"
#include "Client_Define.h"
NS_BEGIN(Client)
class CVillage :
    public CGameObject
{
private:
    CVillage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVillage(const CVillage& prototype);
public:
    virtual ~CVillage() = default;

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

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CModel> m_pModelCom = { nullptr };
    shared_ptr<CTexture> m_pTextureCom = { nullptr };
    shared_ptr<CCollider> m_pColliderCom = { nullptr };


    shared_ptr<class CInventory>  m_pShopCom[ETOI(SHOPTAB::END)];
public:
    static shared_ptr<CVillage> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END