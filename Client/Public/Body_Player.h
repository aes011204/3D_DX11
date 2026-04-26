#pragma once
#include "PartObject.h"
#include "Client_Define.h"

NS_BEGIN(Client)
class CBody_Player :
    public CPartObject
{
public:
    struct BodyPlayer_DESC : public CPartObject::PARTOBJ_DESC
    {

    };
protected:
    CBody_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CBody_Player(const CBody_Player& prototype);
public:
    virtual ~CBody_Player() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

    virtual void OnGui() override;

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();

    const _float4x4* Get_SocketBoneMatrix(const _char* pBoneName);
protected:
    HRESULT Ready_Components();
private:

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CModel> m_pModelCom = { nullptr };
    shared_ptr<CTexture> m_pTextureCom = { nullptr };

    float  emissivePower = 0.f;
protected:
  

protected:

public:
    static shared_ptr<CBody_Player> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<CGameObject> Clone(void* pArg);
    void Free() override;

};

NS_END