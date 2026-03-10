#include "Material.h"

CMaterial::CMaterial(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
}

HRESULT CMaterial::Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath)
{









    return S_OK;
}

shared_ptr<CMaterial> CMaterial::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
	shared_ptr<CMaterial> pInstance(new CMaterial(pDevice, pContext), [](CMaterial* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pAIMaterial, pModelFilePath)))
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
