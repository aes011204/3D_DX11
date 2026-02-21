#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CShader :
    public CComponent
{
private:
	CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CShader(const CShader& Prototype);
public:
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElement);
	virtual HRESULT Initialize(void* pArg) override;


	HRESULT Begin(_uint iPassIndex);
	HRESULT Bind_SRV(const _char* pConstantName, ComPtr<ID3D11ShaderResourceView> pSRV);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);

	virtual void Save_ToJson(nlohmann::json& j) override;
	virtual void Load_FromJson(nlohmann::json& j) override;

private:
	_uint m_iNumPasses = {};
	ComPtr<ID3DX11Effect> m_pEffect = {};

	vector<ComPtr<ID3D11InputLayout>> m_pInputLayouts = {};



public:
	static shared_ptr<CShader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pTextureFilePath, const D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElement);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END