#include "Texture.h"

CTexture::CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& Prototype)
	:CComponent(Prototype)
	, m_iNumSRVs{ Prototype.m_iNumSRVs }
	, m_SRVs{ Prototype.m_SRVs }
{

}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumSRVs)
{
	m_iNumSRVs = iNumSRVs;
	for (int i = 0; i < iNumSRVs;i++)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };


		_tchar szTextureFilePath[MAX_PATH] = {};

		wsprintf(szTextureFilePath, pTextureFilePath, i);

		_tchar szEXT[MAX_PATH] = {};

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT hr = {};
		if(false == lstrcmp( szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateWICTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(pSRV);
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return CComponent::Initialize(pArg);
	return S_OK;

}

shared_ptr<CTexture> CTexture::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pTextureFilePath,
	_uint iNumSRVs)
{
	shared_ptr<CTexture> pInstance (new CTexture(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumSRVs)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
	}
	return pInstance;
}

shared_ptr<CComponent> CTexture::Clone(void* pArg)
{
	shared_ptr<CTexture> pInstance (new CTexture(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
	}
	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	//for (auto& pSRV : m_SRVs)
	//	Safe_Release(pSRV);
	m_SRVs.clear();

}
