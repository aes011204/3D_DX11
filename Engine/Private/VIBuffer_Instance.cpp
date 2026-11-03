#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& Prototype)
	: CVIBuffer{ Prototype }

	, m_instanceBufferDesc{ Prototype.m_instanceBufferDesc }
	, m_instanceStride{ Prototype.m_instanceStride }
	, m_numInstances{ Prototype.m_numInstances }
	, m_indexCountPerInstance{ Prototype.m_indexCountPerInstance }

	, m_VBInstance{ Prototype.m_VBInstance }
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	return S_OK;

}

HRESULT CVIBuffer_Instance::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_VB.Get(),
		m_VBInstance.Get(),
	};

	_uint iVertexStrides[] = {
		m_vertexStride,
		m_instanceStride,
	};

	_uint iOffsets[] = {
		0,0,
	};

	m_context->IASetVertexBuffers(0, m_numVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_context->IASetIndexBuffer(m_IB.Get(), 2 == m_indexStride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_context->IASetPrimitiveTopology(m_primitiveType);


	return S_OK;

}

HRESULT CVIBuffer_Instance::Render()
{

	m_context->DrawIndexedInstanced(m_indexCountPerInstance, m_numInstances, 0, 0, 0);

	return S_OK;
}



void CVIBuffer_Instance::Free()
{
	CVIBuffer::Free();
}
