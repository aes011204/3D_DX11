#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CAssimp_Material :
    public CBase
{
private:
    CAssimp_Material(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
    virtual ~CAssimp_Material() = default;

public:
    HRESULT Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath);
    HRESULT Bind_Material(shared_ptr<class CShader> pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iTextureIndex);
private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

    vector<ComPtr<ID3D11ShaderResourceView>> m_MaterialTextures[AI_TEXTURE_TYPE_MAX];

public:
    static shared_ptr<CAssimp_Material> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath);
    void Free() override;

};

NS_END