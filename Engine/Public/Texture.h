#pragma once
#include "Component.h"

NS_BEGIN(Engine)


class ENGINE_DLL CTexture final :
    public CComponent
{
private:
	CTexture(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CTexture(const CTexture& prototype);
public:
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* textureFilePath, _uint numSRVs);
	virtual HRESULT Initialize(void* arg) override;

	HRESULT Bind_ShaderResourceView(shared_ptr<class CShader> shaderCom, const char* constantName, _uint index);

	_float2 Get_SizeFromSRV(_uint index);
	_uint Get_NumSRVs() {return m_numSRVs;};
private:

private:
	_uint m_numSRVs = {};
	vector<ComPtr<ID3D11ShaderResourceView>> m_SRVs = {};


public:
	static shared_ptr<CTexture> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _tchar* textureFilePath, _uint numSRVs);
	virtual shared_ptr<CComponent> Clone(void* arg) override;
	virtual void Free() override;

};

NS_END
