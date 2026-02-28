#include "Texture.h"
#include "Shader.h"
#include "UITransform.h"

CTexture::CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& Prototype)
	:CComponent(Prototype)
	, m_iNumSRVs{ Prototype.m_iNumSRVs }
	//, m_SRVs{ Prototype.m_SRVs }
{
	m_SRVs = Prototype.m_SRVs;
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumSRVs)
{
	m_iNumSRVs = iNumSRVs;
	for (_uint i = 0; i < iNumSRVs;i++)
	{
		ComPtr<ID3D11ShaderResourceView> pSRV = { nullptr };


		_tchar szTextureFilePath[MAX_PATH] = {};

		wsprintf(szTextureFilePath, pTextureFilePath, i);

		_tchar szEXT[MAX_PATH] = {};

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT hr = {};
		if(false == lstrcmp( szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);
		else if (false == lstrcmp(szEXT, TEXT(".tga")))
			hr = E_FAIL;
		else 
			hr = CreateWICTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(pSRV);
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	HRESULT hr = E_FAIL;


	hr = CComponent::Initialize(pArg);



	return hr;

}

HRESULT CTexture::Bind_ShaderResourceView(shared_ptr<CShader> pShaderCom, const char* pConstantName, _uint iIndex)
{
	return pShaderCom->Bind_SRV(pConstantName, m_SRVs[iIndex]);
}

_float2 CTexture::Get_SizeFromSRV(_uint index)
{
	if (m_SRVs[index] == nullptr&&index >=m_iNumSRVs)
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

shared_ptr<CTexture> CTexture::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pTextureFilePath,
	_uint iNumSRVs)
{
	shared_ptr<CTexture> pInstance(new CTexture(pDevice, pContext), [](CTexture* p) {p->Free();delete p;});

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumSRVs)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
	}
	return pInstance;
}

shared_ptr<CComponent> CTexture::Clone(void* pArg)
{
	shared_ptr<CTexture> pInstance (new CTexture(*this),  [](CTexture* p) {p->Free();delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
	}
	return pInstance;
}

void CTexture::Free()
{

	for (auto& pSRV : m_SRVs)
		pSRV.Reset();

	m_SRVs.clear();

	__super::Free();
}
