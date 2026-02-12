#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	:CComponent(Prototype)
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

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, 0);
	m_pContext->IASetIndexBuffer(0, 2 == m_iIndexStride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	ID3D11InputLayout* pInputLayout = { nullptr };
	//m_pDevice->CreateInputLayout()
	//	m_pContext->IASetInputLayout();

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
