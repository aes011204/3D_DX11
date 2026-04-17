#include "RenderTarget.h"

#include "Shader.h"

CRenderTarget::CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
}

CRenderTarget::CRenderTarget(const CRenderTarget& Prototype)
{
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, m_Texture2D.GetAddressOf())))
		return E_FAIL;


	if (FAILED(m_pDevice->CreateRenderTargetView(m_Texture2D.Get(), nullptr, m_pRTV.GetAddressOf())))
		return E_FAIL;


	if (FAILED(m_pDevice->CreateShaderResourceView(m_Texture2D.Get(), nullptr, m_pSRV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(shared_ptr<CShader> pShader, const _char* pConstantName)
{

	return pShader->Bind_SRV(pConstantName, m_pSRV);
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV.Get(), reinterpret_cast<_float*>(&m_vClearColor));
}

shared_ptr<CRenderTarget> CRenderTarget::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,
	_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	shared_ptr<CRenderTarget> pInstance(new CRenderTarget(pDevice, pContext), [](CRenderTarget* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vColor)))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;

}

void CRenderTarget::Free()
{
	CBase::Free();

	m_pRTV.Reset();
	m_pSRV.Reset();
	m_Texture2D.Reset();

	m_pDevice.Reset();
	m_pContext.Reset();
}
