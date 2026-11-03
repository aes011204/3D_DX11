#pragma once

#pragma once
#include "GameObject.h"
#include "Client_Define.h"
NS_BEGIN(Client)
class CVillage :
    public CGameObject
{
private:
    CVillage(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CVillage(const CVillage& prototype);
public:
    virtual ~CVillage() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Priority_Update(_float timeDelta) override;
    virtual void Update(_float timeDelta) override;
    virtual void Late_Update(_float timeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();

protected:
    HRESULT Ready_Components();
private:

    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CModel> m_modelCom = { nullptr };
    shared_ptr<CTexture> m_textureCom = { nullptr };
    shared_ptr<CCollider> m_colliderCom = { nullptr };


    shared_ptr<class CInventory>  m_shopCom[ETOI(EShopTab::END)];
public:
    static shared_ptr<CVillage> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;



};

NS_END