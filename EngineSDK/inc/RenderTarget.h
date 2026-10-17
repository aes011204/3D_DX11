#pragma once
#include "Base.h"



NS_BEGIN(Engine)

	class CRenderTarget :
	public CBase
{
protected:
	CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CRenderTarget(const CRenderTarget& Prototype);
public:
	virtual ~CRenderTarget() = default;

public:
	
	virtual HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	virtual HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName) ;
	virtual void Clear();

	ComPtr<ID3D11RenderTargetView>	Get_RTV() { return m_pRTV; }
#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(shared_ptr<class CVIBuffer_Rect> pVIBuffer, shared_ptr<class CShader> pShader);
#endif
protected:

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr < ID3D11DeviceContext> m_pContext = { nullptr };

private:

	ComPtr<ID3D11Texture2D> m_Texture2D = { nullptr };
	ComPtr<ID3D11RenderTargetView> m_pRTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView> m_pSRV = { nullptr };

private:
	_float4			m_vClearColor = {};
#ifdef _DEBUG
private:
	_float4x4		m_WorldMatrix = {};
#endif
public:
	static shared_ptr<CRenderTarget> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	virtual void Free() override;


};

NS_END