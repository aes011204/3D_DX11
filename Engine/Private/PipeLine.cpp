#include "PipeLine.h"
#include "Shader.h"

CPipeLine::CPipeLine()
{
}

CPipeLine::~CPipeLine()
{
	Free();
}

HRESULT CPipeLine::Bind_CamPosition(shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_RawValue(pConstantName, Get_CamPosition(), sizeof(_float4));
}

HRESULT CPipeLine::Bind_TransformMatrix(D3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName,&m_TransformStateMatrices[ETOI(eTransformState)]);
}

HRESULT CPipeLine::Bind_TransformMatrix_Inverse(D3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformInverseStateMatrices[ETOI(eTransformState)]);
}

void CPipeLine::Update()
{
	for (int i = 0; i < ETOI(D3DTS::END); i++)
	{
		XMStoreFloat4x4(&m_TransformInverseStateMatrices[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformStateMatrices[i])));
	}
}

unique_ptr<CPipeLine> CPipeLine::Create()
{
	unique_ptr<CPipeLine> pInstance(new CPipeLine());

	/*if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
	}*/
	return pInstance;
}

void CPipeLine::Free()
{
}
