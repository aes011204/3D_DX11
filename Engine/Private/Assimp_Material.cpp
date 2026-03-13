#include "Assimp_Material.h"
#include "Shader.h"

CAssimp_Material::CAssimp_Material(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT CAssimp_Material::Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
	_char szDrive[MAX_PATH] = {};
	_char szDir[MAX_PATH] = {};

	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);


	for(size_t i = 0; i<AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint       iNumTextures = { pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i)) };

		if (0 == iNumTextures)
			continue;

		m_MaterialTextures[i].reserve(iNumTextures);

		for(size_t j =0; j < iNumTextures;j++)
		{
			aiString strTexturePath = {};

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				continue;

			_char szFileName[MAX_PATH] = {};
			_char szEXT[MAX_PATH] = {};

			_splitpath_s(strTexturePath.C_Str(), nullptr,0 , nullptr, 0,
				szFileName , MAX_PATH, szEXT, MAX_PATH);


			_char szFullPath[MAX_PATH] = {};

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			_tchar szTextureFilePath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szTextureFilePath, MAX_PATH);

			HRESULT hr = {};
			ComPtr<ID3D11ShaderResourceView> pSRV = { nullptr };

			if (false == strcmp(szEXT, (".dds")))
				hr = CreateDDSTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

			else if (false == strcmp(szEXT, (".tga")))
				hr = E_FAIL;

			else
				hr = CreateWICTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

			if (FAILED(hr))
				return E_FAIL;

			m_MaterialTextures[i].push_back(pSRV);

		}
	}




    return S_OK;
}

 
HRESULT CAssimp_Material::Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, aiTextureType eMaterialType,
	_uint iTextureIndex)
{
	if (nullptr == m_MaterialTextures[eMaterialType][iTextureIndex] ||
		iTextureIndex >= m_MaterialTextures[eMaterialType].size())
		return E_FAIL;

	pShader->Bind_SRV(pConstantName, m_MaterialTextures[eMaterialType][iTextureIndex]);

	return S_OK;
}

shared_ptr<CAssimp_Material> CAssimp_Material::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
	shared_ptr<CAssimp_Material> pInstance(new CAssimp_Material(pDevice, pContext), [](CAssimp_Material* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pAIMaterial, pModelFilePath)))
	{
		MSG_BOX("Failed to Created : CAssimp_Material");
	}
	return pInstance;
}

void CAssimp_Material::Free()
{
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		m_MaterialTextures[i].clear();
	}

	__super::Free();
}
