#include "Shader.h"

CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& Prototype)
	:CComponent(Prototype)
	, m_iNumPasses{ Prototype.m_iNumPasses }
	, m_pEffect{ Prototype.m_pEffect }
	, m_pInputLayouts{ Prototype.m_pInputLayouts }

{
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElement)
{
	_uint iHlslFlag = {};

#ifdef _DEBUG
	iHlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
	iHlslFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1
#endif

	//if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0,
	//	m_pDevice.Get(), &m_pEffect, nullptr)))
	//	return E_FAIL;
		ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DX11CompileEffectFromFile(
		pShaderFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		iHlslFlag,
		0,
		m_pDevice.Get(),
		&m_pEffect,
		&errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		return E_FAIL;
	}
	ComPtr<ID3DX11EffectTechnique> pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique.Get())
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC TechniqueDesc{};

	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	m_pInputLayouts.reserve(m_iNumPasses);

	for (_uint i = 0; i < m_iNumPasses; i++)
	{
		ComPtr<ID3D11InputLayout> pInputLayOut = {};

		ComPtr<ID3DX11EffectPass> pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass.Get())
			return E_FAIL;

		D3DX11_PASS_DESC PassDesc{};
		pPass->GetDesc(&PassDesc);

		//PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize;

		m_pDevice->CreateInputLayout(pElement, iNumElement,
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
			&pInputLayOut);

		if (nullptr == pInputLayOut.Get())
			return E_FAIL;

		m_pInputLayouts.push_back(pInputLayOut.Get());
	}


    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses ||
		nullptr == m_pInputLayouts[iPassIndex])
		return E_FAIL;

	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext.Get());

	m_pContext->IASetInputLayout(m_pInputLayouts[iPassIndex].Get());


	return S_OK;
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ComPtr<ID3D11ShaderResourceView> pSRV)
{
	//해당 이름을 가진 변수가 있는지 확인
	ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (!pVariable->IsValid())
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 렌더 리소스 뷰인지 확인
	ComPtr<ID3DX11EffectShaderResourceVariable> pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable.Get())
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}
	// 둘다 맞다면 매트릭스 셋팅
	return pSRVVariable->SetResource(pSRV.Get());
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	//매트릭스이름이 있는지 확인
	ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 매트릭스인지 확인
	ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();
	if(nullptr== pMatrixVariable)
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}
	// 둘다 맞다면 매트릭스 셋팅
	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));

	
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	//매트릭스이름이 있는지 확인
	ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 매트릭스인지 확인
	ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}
	// 둘다 맞다면 매트릭스 셋팅
	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrix), 0, iNumMatrices);


}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	//매트릭스이름이 있는지 확인
	ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 매트릭스인지 확인

	return pVariable->SetRawValue(pData, 0,iLength);

}

void CShader::Save_ToJson(nlohmann::json& j)
{


}

void CShader::Load_FromJson(nlohmann::json& j)
{

}

shared_ptr<CShader> CShader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElement)
{
	shared_ptr<CShader> pInstance(new CShader(pDevice, pContext), [](CShader* p) {                                  
		if (nullptr == p)
			return;
		p->Free();
		delete p;
		});

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElement, iNumElement)))
	{
		MSG_BOX("Failed to Cloned : CShader");
	}
	return pInstance;
}

shared_ptr<CComponent> CShader::Clone(void* pArg)
{
	shared_ptr<CShader> pInstance(new CShader(*this), [](CShader* p) {p->Free(); delete p;});
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
	}
	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& it : m_pInputLayouts)
	{
		it.Reset();
	}
	m_pInputLayouts.clear();

	m_pEffect.Reset();


}
