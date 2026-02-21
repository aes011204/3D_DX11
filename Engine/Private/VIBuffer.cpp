#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	:CComponent(Prototype)
	, m_pVB{ Prototype.m_pVB }
	, m_pIB{ Prototype.m_pIB }
	, m_iNumVertexBuffers{ Prototype.m_iNumVertexBuffers }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_ePrimitiveType{ Prototype.m_ePrimitiveType }
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
		m_pVB.Get(),
		// m_pVBInstance, 
	};

	_uint			iVertexStrides[] = {
		m_iVertexStride,

	};

	_uint			iOffsets[] = {
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB.Get(), 2 == m_iIndexStride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

// 이 이후에 쉐이더랑 얘기 해서 버퍼의 위치를 화정

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	return S_OK;

}

void CVIBuffer::Free()
{
	__super::Free();
	m_pVB.Reset();
	m_pIB.Reset();
}
