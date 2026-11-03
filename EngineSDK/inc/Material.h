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
    HRESULT Initialize(ifstream& InFile/*, const _char* pModelFilePath*/);
    HRESULT Bind_Material(shared_ptr<class CShader> pShader, const _char* pConstantName, Cvt_TexType eMaterialType, _uint iTextureIndex);
private:
    ComPtr<ID3D11Device> m_device = { nullptr };
    ComPtr<ID3D11DeviceContext> m_context = { nullptr };
    ComPtr<ID3D11ShaderResourceView> m_blackSRV = { nullptr };
    vector<ComPtr<ID3D11ShaderResourceView>> m_materialTextures[AI_TEXTURE_TYPE_MAX];
    _char m_matName[MAX_PATH];
public:
    static shared_ptr<CMaterial> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, ifstream& InFile/*, const _char* pModelFilePath*/);
    void Free() override;

};

NS_END