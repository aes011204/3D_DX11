#pragma once
#include "Component.h"
class CShader :
    public CComponent
{
private:
	CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CShader(const CShader& Prototype);
public:
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint m_iNumPasses = {};
	ComPtr<ID3DX11Effect> m_pEffect = {nullptr};

public:
	static shared_ptr<CShader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pTextureFilePath);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;

};

