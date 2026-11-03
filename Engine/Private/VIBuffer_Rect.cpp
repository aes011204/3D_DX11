#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CVIBuffer(device, context)
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

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;

	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXTEX* vertices = new VTXTEX[m_numVertices];
	vertices[0].position = _float3(-0.5f, 0.5f, 0.f);
	vertices[0].texcoord = _float2(0.f, 0.f);

	vertices[1].position = _float3(0.5f, 0.5f, 0.f);
	vertices[1].texcoord = _float2(1.f, 0.f);

	vertices[2].position = _float3(0.5f, -0.5f, 0.f);
	vertices[2].texcoord = _float2(1.f, 1.f);

	vertices[3].position = _float3(-0.5f, -0.5f, 0.f);
	vertices[3].texcoord = _float2(0.f, 1.f);


	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;


	//Index Buffer

	D3D11_BUFFER_DESC	indexBufferDesc{};
	indexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = m_indexStride;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	_ushort* indices = new _ushort[m_numIndices];

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = indices;


	if (FAILED(m_device->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_IB)))
		return E_FAIL;


	Safe_Delete_Array(vertices);
	Safe_Delete_Array(indices);


	return S_OK;

}

HRESULT CVIBuffer_Rect::Initialize(void* arg)
{
	return S_OK;

}

shared_ptr<CVIBuffer_Rect> CVIBuffer_Rect::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CVIBuffer_Rect> instance(new CVIBuffer_Rect(device, context), [](CVIBuffer_Rect* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
	}
	return instance;
}

shared_ptr<CComponent> CVIBuffer_Rect::Clone(void* arg)
{
	shared_ptr<CVIBuffer_Rect> instance ( new CVIBuffer_Rect(*this), [](CVIBuffer_Rect* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
	}
	return instance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
