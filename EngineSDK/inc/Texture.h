#pragma once
#include "Component.h"

NS_BEGIN(Engine)


class ENGINE_DLL CTexture final :
    public CComponent
{
private:
	CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTexture(const CTexture& Prototype);
public:
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumSRVs);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Bind_ShaderResourceView(shared_ptr<class CShader> pShaderCom, const char* pConstantName, _uint iIndex);

	_float2 Get_SizeFromSRV(_uint index);
	_uint Get_NumSRVs() {return m_iNumSRVs;};
private:

private:
	_uint m_iNumSRVs = {};
	vector<ComPtr<ID3D11ShaderResourceView>> m_SRVs = {};


public:
	static shared_ptr<CTexture> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pTextureFilePath, _uint iNumSRVs);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
