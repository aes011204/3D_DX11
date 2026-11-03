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

HRESULT CPipeLine::Bind_TransformMatrix(ED3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName,&m_transformStateMatrices[ETOI(eTransformState)]);
}

HRESULT CPipeLine::Bind_TransformMatrix_Inverse(ED3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_transformInverseStateMatrices[ETOI(eTransformState)]);
}

void CPipeLine::Update()
{
	for (int i = 0; i < ETOI(ED3DTS::END); i++)
	{
		XMStoreFloat4x4(&m_transformInverseStateMatrices[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_transformStateMatrices[i])));
	}
}

void CPipeLine::OnGui()
{

	CBase::OnGui();
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
