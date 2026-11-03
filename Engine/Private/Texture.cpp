#include "Texture.h"
#include "Shader.h"
#include "UITransform.h"

CTexture::CTexture(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CComponent(device, context)
{
}

CTexture::CTexture(const CTexture& Prototype)
	:CComponent(Prototype)
	, m_numSRVs{ Prototype.m_numSRVs }
	, m_SRVs{ Prototype.m_SRVs }
{
	

}

HRESULT CTexture::Initialize_Prototype(const _tchar* textureFilePath, _uint numSRVs)
{
	m_numSRVs = numSRVs;
	for (_uint i = 0; i < numSRVs;i++)
	{
		ComPtr<ID3D11ShaderResourceView> SRV = { nullptr };


		_tchar szTextureFilePath[MAX_PATH] = {};

		wsprintf(szTextureFilePath, textureFilePath, i);

		_tchar szEXT[MAX_PATH] = {};

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT hr = {};
		if(false == lstrcmp( szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_device.Get(), szTextureFilePath, nullptr, &SRV);
		else if (false == lstrcmp(szEXT, TEXT(".tga")))
			hr = E_FAIL;
		else 
			hr = CreateWICTextureFromFile(m_device.Get(), szTextureFilePath, nullptr, &SRV);

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(SRV);
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void* arg)
{
	HRESULT hr = E_FAIL;


	hr = CComponent::Initialize(arg);



	return hr;

}

HRESULT CTexture::Bind_ShaderResourceView(shared_ptr<CShader> shaderCom, const char* constantName, _uint index)
{
	return shaderCom->Bind_SRV(constantName, m_SRVs[index]);
}

_float2 CTexture::Get_SizeFromSRV(_uint index)
{
	if (m_SRVs[index] == nullptr&&index >=m_numSRVs)
	{
		MSG_BOX("m_SRV[index] is nullptr");
		return { 0,0 };
	}
	//
	ComPtr<ID3D11Resource> resource;
	m_SRVs[index]->GetResource(resource.GetAddressOf());

	ComPtr<ID3D11Texture2D> tex2d;
	resource.As(&tex2d);

	D3D11_TEXTURE2D_DESC desc;
	tex2d->GetDesc(&desc);


	return {static_cast<_float>(desc.Width), static_cast<_float>(desc.Height)};
}

shared_ptr<CTexture> CTexture::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _tchar* textureFilePath,
	_uint numSRVs)
{
	shared_ptr<CTexture> instance(new CTexture(device, context), [](CTexture* p) {p->Free();delete p;});

	if (FAILED(instance->Initialize_Prototype(textureFilePath, numSRVs)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		return nullptr;
	}
	return instance;
}

shared_ptr<CComponent> CTexture::Clone(void* arg)
{
	shared_ptr<CTexture> instance (new CTexture(*this),[](CTexture* p){p->Free();delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		return nullptr;
	}
	return instance;
}

void CTexture::Free()
{

	for (auto& SRV : m_SRVs)
		SRV.Reset();

	m_SRVs.clear();

	__super::Free();
}
