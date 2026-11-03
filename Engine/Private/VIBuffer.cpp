#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	:CComponent(Prototype)
	, m_VB{ Prototype.m_VB }
	, m_IB{ Prototype.m_IB }
	, m_numVertexBuffers{ Prototype.m_numVertexBuffers }
	, m_vertexStride{ Prototype.m_vertexStride }
	, m_numVertices{ Prototype.m_numVertices }
	, m_indexStride{ Prototype.m_indexStride }
	, m_numIndices{ Prototype.m_numIndices }
	, m_primitiveType{ Prototype.m_primitiveType }
{
}

HRESULT CVIBuffer::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;

}

HRESULT CVIBuffer::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_VB.Get(),
		// m_pVBInstance, 
	};

	_uint			iVertexStrides[] = {
		m_vertexStride,

	};

	_uint			iOffsets[] = {
		0
	};

	m_context->IASetVertexBuffers(0, m_numVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_context->IASetIndexBuffer(m_IB.Get(), 2 == m_indexStride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_context->IASetPrimitiveTopology(m_primitiveType);

// 이 이후에 쉐이더랑 얘기 해서 버퍼의 위치를 화정

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_context->DrawIndexed(m_numIndices, 0, 0);
	return S_OK;

}

void CVIBuffer::Free()
{
	__super::Free();
	m_VB.Reset();
	m_IB.Reset();
}
