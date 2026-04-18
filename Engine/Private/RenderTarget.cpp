#include "RenderTarget.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

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
#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint iNumViewPorts = { 1 };
	D3D11_VIEWPORT ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;

	m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(shared_ptr<CVIBuffer_Rect> pVIBuffer, shared_ptr < CShader> pShader)
{
	pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

	pShader->Bind_SRV("g_Texture", m_pSRV);

	pShader->Begin(0);

	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	return S_OK;
}
#endif
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
