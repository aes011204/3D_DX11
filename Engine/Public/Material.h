#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CMaterial :
    public CBase
{
private:
    CMaterial(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
    virtual ~CMaterial() = default;

public:
    HRESULT Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath);

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

    vector<ComPtr<ID3D11ShaderResourceView>> m_MaterialTextures[AI_TEXTURE_TYPE_MAX];

public:
    static shared_ptr<CMaterial> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath);
    void Free() override;

};

NS_END