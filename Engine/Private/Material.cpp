#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT CMaterial::Initialize(ifstream& InFile/*, const _char* pModelFilePath*/)
{
	/*_char szDrive[MAX_PATH] = {};
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
	}*/


		// 2. 재질 이름 읽기 (MAX_PATH만큼 고정 크기)
		_char szMatName[MAX_PATH] = {};
		InFile.read(szMatName, MAX_PATH);

		memcpy(&m_szMatName, &szMatName, MAX_PATH);
		//// 엔진에서 사용할 재질 객체 생성
		//auto pMaterial = make_shared<CMaterial>(m_pDevice, m_pContext);
		//pMaterial->Set_Name(szMatName);

		// 3. 텍스처 타입 루프 (컨버터의 AI_TEXTURE_TYPE_MAX와 횟수가 같아야 함)
		// 보통 Assimp 5.x 기준 18입니다. 컨버터에서 출력된 값을 보고 맞추세요.
		/*const _uint iAssimpTypeMax = 18;*/

		for (_uint j = 0; j < MAX_TEXTURE_SLOT; ++j)
		{
			// 4. 이 타입(j)의 텍스처 개수 읽기
			_uint iNumTextures = 0;
			if (!InFile.read(reinterpret_cast<char*>(&iNumTextures), sizeof(_uint)))
				break;

			if (0 == iNumTextures)
				continue;

			// 5. 개수만큼 경로 읽고 텍스처 로드
			for (_uint k = 0; k < iNumTextures; ++k)
			{
				_char szFullPath[MAX_PATH] = {};
				if (!InFile.read(szFullPath, MAX_PATH))
					break;

				// 유니코드 변환
				_tchar szWPath[MAX_PATH] = {};
				MultiByteToWideChar(CP_ACP, 0, szFullPath, -1, szWPath, MAX_PATH);

				// 실제 텍스처 로드 및 SRV 생성 (기존 함수 활용)
				ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;

				// 확장자 체크 로직
				HRESULT hr = S_OK;
				if (strstr(szFullPath, ".dds") || strstr(szFullPath, ".DDS"))
					hr = CreateDDSTextureFromFile(m_pDevice.Get(), szWPath, nullptr, &pSRV);
				else
					hr = CreateWICTextureFromFile(m_pDevice.Get(), szWPath, nullptr, &pSRV);

				if (SUCCEEDED(hr))
				{
					// 재질 객체 내부의 벡터 배열에 추가
					// pMaterial->m_Textures[j].push_back(pSRV); 와 같은 형태
					//pMaterial->Add_Texture(j, pSRV);

					m_MaterialTextures[j].push_back(pSRV);
				}
			}
		}

	


    return S_OK;
}

 
HRESULT CMaterial::Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, Cvt_TexType eMaterialType,
	_uint iTextureIndex)
{
	//if (nullptr == m_MaterialTextures[eMaterialType][iTextureIndex] ||
	//	iTextureIndex >= m_MaterialTextures[eMaterialType].size())
	//	return E_FAIL;
	if (iTextureIndex >= m_MaterialTextures[eMaterialType].size() ||
		nullptr == m_MaterialTextures[eMaterialType][iTextureIndex])
		return E_FAIL;
	pShader->Bind_SRV(pConstantName, m_MaterialTextures[eMaterialType][iTextureIndex]);

	return S_OK;
}

shared_ptr<CMaterial> CMaterial::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, ifstream& InFile/*, const _char* pModelFilePath*/)
{
	shared_ptr<CMaterial> pInstance(new CMaterial(pDevice, pContext), [](CMaterial* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(InFile/*, pModelFilePath*/)))
	{
		MSG_BOX("Failed to Created : CMaterial");
	}
	return pInstance;
}

void CMaterial::Free()
{
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		m_MaterialTextures[i].clear();
	}

	__super::Free();
}
