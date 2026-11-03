#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& Prototype)
	: CVIBuffer(Prototype)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_numVertexBuffers = 1; // 버택스 버퍼가 몇갠지 사각형그리는 거니까 1개

	m_numVertices = 4;
	m_vertexStride = sizeof(VTXTEX);

	m_numIndices = 6;
	m_indexStride = 2; // 2바이트랑 4바이트 중 택1

	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXTEX* pVertices = new VTXTEX[m_numVertices];
	pVertices[0].position = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].texcoord = _float2(0.f, 0.f);

	pVertices[1].position = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].texcoord = _float2(1.f, 0.f);

	pVertices[2].position = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].texcoord = _float2(1.f, 1.f);

	pVertices[3].position = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].texcoord = _float2(0.f, 1.f);


	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
		return E_FAIL;


	//Index Buffer

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_indexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_numIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;


	if (FAILED(m_device->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_IB)))
		return E_FAIL;


	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


	return S_OK;

}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
	return S_OK;

}

shared_ptr<CVIBuffer_Rect> CVIBuffer_Rect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CVIBuffer_Rect> pInstance(new CVIBuffer_Rect(pDevice, pContext), [](CVIBuffer_Rect* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Rect::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_Rect> pInstance ( new CVIBuffer_Rect(*this), [](CVIBuffer_Rect* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
	}
	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
