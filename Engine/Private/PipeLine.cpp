#include "PipeLine.h"
#include "Shader.h"

CPipeLine::CPipeLine()
{
}

CPipeLine::~CPipeLine()
{
	Free();
}

HRESULT CPipeLine::Bind_CamPosition(shared_ptr<class CShader> shader, const _char* constantName)
{
	return shader->Bind_RawValue(constantName, Get_CamPosition(), sizeof(_float4));
}

HRESULT CPipeLine::Bind_TransformMatrix(ED3DTS transformState, shared_ptr<class CShader> shader, const _char* constantName)
{
	return shader->Bind_Matrix(constantName,&m_transformStateMatrices[ETOI(transformState)]);
}

HRESULT CPipeLine::Bind_TransformMatrix_Inverse(ED3DTS transformState, shared_ptr<class CShader> shader, const _char* constantName)
{
	return shader->Bind_Matrix(constantName, &m_transformInverseStateMatrices[ETOI(transformState)]);
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
	unique_ptr<CPipeLine> instance(new CPipeLine());

	/*if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
	}*/
	return instance;
}

void CPipeLine::Free()
{
}
