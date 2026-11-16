#include "Shader.h"

CShader::CShader(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CComponent(device, context)
{
}

CShader::CShader(const CShader& Prototype)
	:CComponent(Prototype)
	, m_numPasses{ Prototype.m_numPasses }
	, m_effect{ Prototype.m_effect }
	, m_inputLayouts{ Prototype.m_inputLayouts }

{
}

HRESULT CShader::Initialize_Prototype(const _tchar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* element, _uint numElement)
{
	_uint hlslFlag = {};

#ifdef _DEBUG
	hlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
	hlslFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1
#endif

	//if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0,
	//	m_pDevice.Get(), &m_pEffect, nullptr)))
	//	return E_FAIL;
		ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DX11CompileEffectFromFile(
		shaderFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		hlslFlag,
		0,
		m_device.Get(),
		&m_effect,
		&errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		return E_FAIL;
	}
	ComPtr<ID3DX11EffectTechnique> technique = m_effect->GetTechniqueByIndex(0);
	if (nullptr == technique.Get())
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC techniqueDesc{};

	technique->GetDesc(&techniqueDesc);

	m_numPasses = techniqueDesc.Passes;

	m_inputLayouts.reserve(m_numPasses);

	for (_uint i = 0; i < m_numPasses; i++)
	{
		ComPtr<ID3D11InputLayout> inputLayOut = {};

		ComPtr<ID3DX11EffectPass> pass = technique->GetPassByIndex(i);
		if (nullptr == pass.Get())
			return E_FAIL;

		D3DX11_PASS_DESC passDesc{};
		pass->GetDesc(&passDesc);

		//PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize;

		m_device->CreateInputLayout(element, numElement,
			passDesc.pIAInputSignature, passDesc.IAInputSignatureSize,
			&inputLayOut);

		if (nullptr == inputLayOut.Get())
			return E_FAIL;

		m_inputLayouts.push_back(inputLayOut.Get());
	}


    return S_OK;
}

HRESULT CShader::Initialize(void* arg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint passIndex)
{
	if (passIndex >= m_numPasses ||
		nullptr == m_inputLayouts[passIndex])
		return E_FAIL;

	m_effect->GetTechniqueByIndex(0)->GetPassByIndex(passIndex)->Apply(0, m_context.Get());

	m_context->IASetInputLayout(m_inputLayouts[passIndex].Get());


	return S_OK;
}

HRESULT CShader::Bind_SRV(const _char* constantName, ComPtr<ID3D11ShaderResourceView> SRV)
{
	//해당 이름을 가진 변수가 있는지 확인
	ComPtr<ID3DX11EffectVariable> variable = m_effect->GetVariableByName(constantName);
	if (!variable->IsValid())
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 렌더 리소스 뷰인지 확인
	ComPtr<ID3DX11EffectShaderResourceVariable> SRVVariable = variable->AsShaderResource();
	if (nullptr == SRVVariable.Get())
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}
	// 둘다 맞다면 매트릭스 셋팅
	return SRVVariable->SetResource(SRV.Get());
}

HRESULT CShader::Bind_Matrix(const _char* constantName, const _float4x4* matrix)
{
	//매트릭스이름이 있는지 확인
	ComPtr<ID3DX11EffectVariable> variable = m_effect->GetVariableByName(constantName);
	if (nullptr == variable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 매트릭스인지 확인
	ComPtr<ID3DX11EffectMatrixVariable> matrixVariable = variable->AsMatrix();
	if(nullptr== matrixVariable)
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}
	// 둘다 맞다면 매트릭스 셋팅
	return matrixVariable->SetMatrix(reinterpret_cast<const _float*>(matrix));

	
}

HRESULT CShader::Bind_Matrices(const _char* constantName, const _float4x4* matrix, _uint numMatrices)
{
	//매트릭스이름이 있는지 확인
	ComPtr<ID3DX11EffectVariable> variable = m_effect->GetVariableByName(constantName);
	if (nullptr == variable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 매트릭스인지 확인
	ComPtr<ID3DX11EffectMatrixVariable> matrixVariable = variable->AsMatrix();
	if (nullptr == matrixVariable)
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}
	// 둘다 맞다면 매트릭스 셋팅
	return matrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(matrix), 0, numMatrices);


}

HRESULT CShader::Bind_RawValue(const _char* constantName, const void* data, _uint length)
{
	//매트릭스이름이 있는지 확인
	ComPtr<ID3DX11EffectVariable> variable = m_effect->GetVariableByName(constantName);
	if (nullptr == variable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}
	// 그 이름을 가진 변수가 매트릭스인지 확인

	return variable->SetRawValue(data, 0,length);

}

void CShader::Save_ToJson(nlohmann::json& j)
{


}

void CShader::Load_FromJson(nlohmann::json& j)
{

}

shared_ptr<CShader> CShader::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _tchar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* element, _uint numElement)
{
	shared_ptr<CShader> instance(new CShader(device, context), [](CShader* p) {                                  
		if (nullptr == p)
			return;
		p->Free();
		delete p;
		});

	if (FAILED(instance->Initialize_Prototype(shaderFilePath, element, numElement)))
	{
		MSG_BOX("Failed to Cloned : CShader");
	}
	return instance;
}

shared_ptr<CComponent> CShader::Clone(void* arg)
{
	shared_ptr<CShader> instance(new CShader(*this), [](CShader* p) {p->Free(); delete p;});
	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
	}
	return instance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& it : m_inputLayouts)
	{
		it.Reset();
	}
	m_inputLayouts.clear();

	m_effect.Reset();


}
