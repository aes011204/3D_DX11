#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CVIBuffer(device, context)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& Prototype)
	: CVIBuffer(Prototype)
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_numVertexBuffers = 1; // 버택스 버퍼가 몇갠지 사각형그리는 거니까 1개

	m_numVertices = 8;
	m_vertexStride = sizeof(VTXCUBE);

	m_numIndices = 36;
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


	VTXCUBE* vertices = new VTXCUBE[m_numVertices];
	vertices[0].position = _float3(-0.5f, 0.5f, -0.5f);
	vertices[0].texcoord = vertices[0].position;

	vertices[1].position = _float3(0.5f, 0.5f, -0.5f);
	vertices[1].texcoord = vertices[1].position;

	vertices[2].position = _float3(0.5f, -0.5f, -0.5f);
	vertices[2].texcoord = vertices[2].position;

	vertices[3].position = _float3(-0.5f, -0.5f, -0.5f);
	vertices[3].texcoord = vertices[3].position;


	vertices[4].position = _float3(-0.5f, 0.5f, 0.5f);
	vertices[4].texcoord = vertices[4].position;

	vertices[5].position = _float3(0.5f, 0.5f, 0.5f);
	vertices[5].texcoord = vertices[5].position;

	vertices[6].position = _float3(0.5f, -0.5f, 0.5f);
	vertices[6].texcoord = vertices[6].position;

	vertices[7].position = _float3(-0.5f, -0.5f, 0.5f);
	vertices[7].texcoord = vertices[7].position;


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
	
	indices[0] = 1; indices[1] = 5; indices[2] = 6;
	indices[3] = 1; indices[4] = 6; indices[5] = 2;

	indices[6] = 4; indices[7] = 0; indices[8] = 3;
	indices[9] = 4; indices[10] = 3; indices[11] = 7;

	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	indices[24] = 7; indices[25] = 6; indices[26] = 5;
	indices[27] = 7; indices[28] = 5; indices[29] = 4;

	indices[30] = 0; indices[31] = 1; indices[32] = 2;
	indices[33] = 0; indices[34] = 2; indices[35] = 3;



	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = indices;


	if (FAILED(m_device->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_IB)))
		return E_FAIL;


	Safe_Delete_Array(vertices);
	Safe_Delete_Array(indices);


	return S_OK;

}

HRESULT CVIBuffer_Cube::Initialize(void* arg)
{
	return S_OK;

}

shared_ptr<CVIBuffer_Cube> CVIBuffer_Cube::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CVIBuffer_Cube> instance(new CVIBuffer_Cube(device, context), [](CVIBuffer_Cube* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
	}
	return instance;
}

shared_ptr<CComponent> CVIBuffer_Cube::Clone(void* arg)
{
	shared_ptr<CVIBuffer_Cube> instance(new CVIBuffer_Cube(*this), [](CVIBuffer_Cube* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
	}
	return instance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
