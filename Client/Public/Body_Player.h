#pragma once
#include "PartObject.h"
#include "Client_Define.h"

NS_BEGIN(Client)
class CBody_Player :
    public CPartObject
{
public:
    struct BodyPlayerDesc : public CPartObject::PartObjDesc
    {

    };
protected:
    CBody_Player(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CBody_Player(const CBody_Player& prototype);
public:
    virtual ~CBody_Player() = default;
public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* arg);
    virtual void Priority_Update(_float timeDelta);
    virtual void Update(_float timeDelta);
    virtual void Late_Update(_float timeDelta);
    virtual HRESULT Render();

    virtual void OnGui() override;

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();

    const _float4x4* Get_SocketBoneMatrix(const _char* boneName);
protected:
    HRESULT Ready_Components();
private:

    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CModel> m_modelCom = { nullptr };
    shared_ptr<CTexture> m_textureCom = { nullptr };

    float  m_emissivePower = 0.f;
protected:
  

protected:

public:
    static shared_ptr<CBody_Player> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    virtual shared_ptr<CGameObject> Clone(void* arg);
    void Free() override;

};

NS_END