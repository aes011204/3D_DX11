#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CAssimp_Material :
    public CBase
{
private:
    CAssimp_Material(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
    virtual ~CAssimp_Material() = default;

public:
    HRESULT Initialize(const aiMaterial* srcAIMaterial, const _char* modelFilePath);
    HRESULT Bind_Material(shared_ptr<class CShader> shader, const _char* constantName, aiTextureType materialType, _uint textureIndex);
private:
    ComPtr<ID3D11Device> m_device = { nullptr };
    ComPtr<ID3D11DeviceContext> m_context = { nullptr };

    vector<ComPtr<ID3D11ShaderResourceView>> m_materialTextures[AI_TEXTURE_TYPE_MAX];

public:
    static shared_ptr<CAssimp_Material> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const aiMaterial* srcAIMaterial, const _char* modelFilePath);
    void Free() override;

};

NS_END