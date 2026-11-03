#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CShader :
    public CComponent
{
private:
	CShader(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CShader(const CShader& prototype);
public:
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* element, _uint numElement);
	virtual HRESULT Initialize(void* arg) override;


	HRESULT Begin(_uint passIndex);
	HRESULT Bind_SRV(const _char* constantName, ComPtr<ID3D11ShaderResourceView> SRV);
	HRESULT Bind_Matrix(const _char* constantName, const _float4x4* matrix);
	HRESULT Bind_Matrices(const _char* constantName, const _float4x4* matrix, _uint numMatrices);
	HRESULT Bind_RawValue(const _char* constantName, const void* matrix, _uint length);


	virtual void Save_ToJson(nlohmann::json& j) override;
	virtual void Load_FromJson(nlohmann::json& j) override;

private:
	_uint m_numPasses = {};
	ComPtr<ID3DX11Effect> m_effect = {};

	vector<ComPtr<ID3D11InputLayout>> m_inputLayouts = {};

public:
	static shared_ptr<CShader> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _tchar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* element, _uint numElement);
	virtual shared_ptr<CComponent> Clone(void* arg) override;
	virtual void Free() override;

};

NS_END