#include "Assimp_Material.h"
#include "Shader.h"

CAssimp_Material::CAssimp_Material(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: m_device(device),m_context(context)
{
}

HRESULT CAssimp_Material::Initialize(const aiMaterial* srcAIMaterial, const _char* modelFilePath)
{
	_char szDrive[MAX_PATH] = {};
	_char szDir[MAX_PATH] = {};

	_splitpath_s(modelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);


	for(size_t i = 0; i<AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint       numTextures = { srcAIMaterial->GetTextureCount(static_cast<aiTextureType>(i)) };

		if (0 == numTextures)
			continue;

		m_materialTextures[i].reserve(numTextures);

		for(size_t j =0; j < numTextures;j++)
		{
			aiString texturePath = {};

			if (FAILED(srcAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &texturePath)))
				continue;

			_char szFileName[MAX_PATH] = {};
			_char szEXT[MAX_PATH] = {};

			_splitpath_s(texturePath.C_Str(), nullptr,0 , nullptr, 0,
				szFileName , MAX_PATH, szEXT, MAX_PATH);


			_char szFullPath[MAX_PATH] = {};

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			_tchar szTextureFilePath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szTextureFilePath, MAX_PATH);

			HRESULT hr = {};
			ComPtr<ID3D11ShaderResourceView> SRV = { nullptr };

			if (false == strcmp(szEXT, (".dds")))
				hr = CreateDDSTextureFromFile(m_device.Get(), szTextureFilePath, nullptr, &SRV);

			else if (false == strcmp(szEXT, (".tga")))
				hr = E_FAIL;

			else
				hr = CreateWICTextureFromFile(m_device.Get(), szTextureFilePath, nullptr, &SRV);

			if (FAILED(hr))
				return E_FAIL;

			m_materialTextures[i].push_back(SRV);

		}
	}




    return S_OK;
}

 
HRESULT CAssimp_Material::Bind_Material(shared_ptr<CShader> shader, const _char* constantName, aiTextureType materialType,
	_uint textureIndex)
{
	if (nullptr == m_materialTextures[materialType][textureIndex] ||
		textureIndex >= m_materialTextures[materialType].size())
		return E_FAIL;

	shader->Bind_SRV(constantName, m_materialTextures[materialType][textureIndex]);

	return S_OK;
}

shared_ptr<CAssimp_Material> CAssimp_Material::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const aiMaterial* srcAIMaterial, const _char* modelFilePath)
{
	shared_ptr<CAssimp_Material> instance(new CAssimp_Material(device, context), [](CAssimp_Material* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(srcAIMaterial, modelFilePath)))
	{
		MSG_BOX("Failed to Created : CAssimp_Material");
	}
	return instance;
}

void CAssimp_Material::Free()
{
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		m_materialTextures[i].clear();
	}

	__super::Free();
}
