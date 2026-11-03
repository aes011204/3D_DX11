#include "RenderTarget.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: m_device(device), m_context(context)
{
}

CRenderTarget::CRenderTarget(const CRenderTarget& prototype)
{
}

HRESULT CRenderTarget::Initialize(_uint sizeX, _uint sizeY, DXGI_FORMAT pixelFormat, const _float4& color)
{

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = sizeX;
	textureDesc.Height = sizeY;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = pixelFormat;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(m_device->CreateTexture2D(&textureDesc, nullptr, m_texture2D.GetAddressOf())))
		return E_FAIL;


	if (FAILED(m_device->CreateRenderTargetView(m_texture2D.Get(), nullptr, m_RTV.GetAddressOf())))
		return E_FAIL;


	if (FAILED(m_device->CreateShaderResourceView(m_texture2D.Get(), nullptr, m_SRV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(shared_ptr<CShader> shader, const _char* constantName)
{

	return shader->Bind_SRV(constantName, m_SRV);
}

void CRenderTarget::Clear()
{
	m_context->ClearRenderTargetView(m_RTV.Get(), reinterpret_cast<_float*>(&m_clearColor));
}
#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float x, _float y, _float sizeX, _float sizeY)
{
	_uint numViewPorts = { 1 };
	D3D11_VIEWPORT viewportDesc{};
	m_context->RSGetViewports(&numViewPorts, &viewportDesc);

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	m_worldMatrix._11 = sizeX;
	m_worldMatrix._22 = sizeY;

	m_worldMatrix._41 = x - viewportDesc.Width * 0.5f;
	m_worldMatrix._42 = -y + viewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(shared_ptr<CVIBuffer_Rect> VIBuffer, shared_ptr < CShader> shader)
{
	shader->Bind_Matrix("g_WorldMatrix", &m_worldMatrix);

	shader->Bind_SRV("g_Texture", m_SRV);

	shader->Begin(0);

	VIBuffer->Bind_Resources();
	VIBuffer->Render();

	return S_OK;
}
#endif
shared_ptr<CRenderTarget> CRenderTarget::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
                                                _uint sizeX, _uint sizeY, DXGI_FORMAT pixelFormat, const _float4& color)
{
	shared_ptr<CRenderTarget> instance(new CRenderTarget(device, context), [](CRenderTarget* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize(sizeX, sizeY, pixelFormat, color)))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return instance;

}

void CRenderTarget::Free()
{
	CBase::Free();

	m_RTV.Reset();
	m_SRV.Reset();
	m_texture2D.Reset();

	m_device.Reset();
	m_context.Reset();
}
