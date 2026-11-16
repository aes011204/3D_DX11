#pragma once
#include "GameObject.h"
#include "Client_Define.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer;
class CTexture;
NS_END

NS_BEGIN(Client)
class CTerrain final:
    public CGameObject {
public:
    struct TerrainDesc final : public CGameObject::GameObjectDesc
    {

    };
private:
    CTerrain(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CTerrain(const CTerrain& prototype);
public:
    virtual ~CTerrain() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Priority_Update(_float timeDelta) override;
    virtual void Update(_float timeDelta) override;
    virtual void Late_Update(_float timeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

protected:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();
private:
    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CVIBuffer> m_VIBufferCom = { nullptr };
    shared_ptr<CTexture> m_textureCom = { nullptr };

public:
    static shared_ptr<CTerrain> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;

};

NS_END