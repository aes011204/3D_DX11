#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
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

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음


	VTXCUBE* pVertices = new VTXCUBE[m_numVertices];
	pVertices[0].position = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].texcoord = pVertices[0].position;

	pVertices[1].position = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].texcoord = pVertices[1].position;

	pVertices[2].position = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].texcoord = pVertices[2].position;

	pVertices[3].position = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].texcoord = pVertices[3].position;


	pVertices[4].position = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].texcoord = pVertices[4].position;

	pVertices[5].position = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].texcoord = pVertices[5].position;

	pVertices[6].position = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].texcoord = pVertices[6].position;

	pVertices[7].position = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].texcoord = pVertices[7].position;


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
	
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	pIndices[24] = 7; pIndices[25] = 6; pIndices[26] = 5;
	pIndices[27] = 7; pIndices[28] = 5; pIndices[29] = 4;

	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;



	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;


	if (FAILED(m_device->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_IB)))
		return E_FAIL;


	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


	return S_OK;

}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;

}

shared_ptr<CVIBuffer_Cube> CVIBuffer_Cube::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CVIBuffer_Cube> pInstance(new CVIBuffer_Cube(pDevice, pContext), [](CVIBuffer_Cube* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Cube::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_Cube> pInstance(new CVIBuffer_Cube(*this), [](CVIBuffer_Cube* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
	}
	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
