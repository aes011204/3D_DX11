#include "Shader.h"

CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& Prototype)
	:CComponent(Prototype)

{
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath)
{
	_uint iHlslFlag = {};

#ifdef _DEBUG
	iHlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
	iHlslFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0,
		m_pDevice.Get(), &m_pEffect, nullptr)))
		return E_FAIL;

	ComPtr<ID3DX11EffectTechnique> pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC TechniqueDesc{};

	pTechnique->GetDesc(&TechniqueDesc);
	for (size_t i = 0; i < m_iNumPasses; i++)
	{
		ComPtr<ID3DX11EffectPass> pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC PassDesc{};
		pPass->GetDesc(&PassDesc);

		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize;
	}


    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}

shared_ptr<CShader> CShader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pTextureFilePath)
{
	shared_ptr<CShader> pInstance(new CShader(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
	}
	return pInstance;
}

shared_ptr<CComponent> CShader::Clone(void* pArg)
{
	shared_ptr<CShader> pInstance(new CShader(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
	}
	return pInstance;
}

void CShader::Free()
{
}
