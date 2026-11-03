#pragma once
#include "Base.h"



NS_BEGIN(Engine)

	class CRenderTarget :
	public CBase
{
protected:
	CRenderTarget(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CRenderTarget(const CRenderTarget& prototype);
public:
	virtual ~CRenderTarget() = default;

public:
	
	virtual HRESULT Initialize(_uint sizeX, _uint sizeY, DXGI_FORMAT pixelFormat, const _float4& color);
	virtual HRESULT Bind_ShaderResource(shared_ptr<class CShader> shader, const _char* constantName) ;
	virtual void Clear();

	ComPtr<ID3D11RenderTargetView>	Get_RTV() { return m_RTV; }
#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float x, _float y, _float sizeX, _float sizeY);
	HRESULT Render(shared_ptr<class CVIBuffer_Rect> VIBuffer, shared_ptr<class CShader> shader);
#endif
protected:

private:
	ComPtr<ID3D11Device> m_device = { nullptr };
	ComPtr < ID3D11DeviceContext> m_context = { nullptr };

private:

	ComPtr<ID3D11Texture2D> m_texture2D = { nullptr };
	ComPtr<ID3D11RenderTargetView> m_RTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView> m_SRV = { nullptr };

private:
	_float4			m_clearColor = {};
#ifdef _DEBUG
private:
	_float4x4		m_worldMatrix = {};
#endif
public:
	static shared_ptr<CRenderTarget> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, 
		_uint sizeX, _uint sizeY, DXGI_FORMAT pixelFormat, const _float4& color);
	virtual void Free() override;


};

NS_END