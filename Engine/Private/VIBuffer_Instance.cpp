#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& Prototype)
	: CVIBuffer{ Prototype }

	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_iInstanceStride{ Prototype.m_iInstanceStride }
	, m_iNumInstances{ Prototype.m_iNumInstances }
	, m_iIndexCountPerInstance{ Prototype.m_iIndexCountPerInstance }

	, m_pVBInstance{ Prototype.m_pVBInstance }
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
		m_pVB.Get(),
		m_pVBInstance.Get(),
	};

	_uint iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint iOffsets[] = {
		0,0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB.Get(), 2 == m_iIndexStride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);


	return S_OK;

}

HRESULT CVIBuffer_Instance::Render()
{

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

	return S_OK;
}



void CVIBuffer_Instance::Free()
{
	CVIBuffer::Free();
}
