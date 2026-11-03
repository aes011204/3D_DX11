#include "VIBuffer_Sea.h"

#include "GameInstance.h"
#include "Shader.h"

CVIBuffer_Sea::CVIBuffer_Sea(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, _uint gridSize, _uint numLevel)
	:CVIBuffer(device, context) ,m_gridSize(gridSize), m_numLevel(numLevel)
{
}

CVIBuffer_Sea::CVIBuffer_Sea(const CVIBuffer_Sea& prototype)
	: CVIBuffer(prototype),
	m_numLevel(prototype.m_numLevel), m_gridSize(prototype.m_gridSize),
m_numRingIndices(prototype.m_numRingIndices), m_numCenterIndices(prototype.m_numCenterIndices)
{
	m_vtxPos = new _float3[m_numVertices];
	memcpy(m_vtxPos, prototype.m_vtxPos, sizeof(_float3) * m_numVertices);
}

HRESULT CVIBuffer_Sea::Initialize_Prototype(/*const _tchar* pHeightMapFilePath*/)
{
	// 무조건 4의 배수여야 함
	if (m_gridSize % 4 != 0)
	{
		MSG_BOX("wrong size");
		return E_FAIL;
	}

	m_numVertexBuffers = 1; // 버택스 버퍼가 몇갠지 사각형그리는 거니까 1개
	m_numVertices = (m_gridSize+1) * (m_gridSize+1);
	m_vertexStride = sizeof(VTXPOS);
	//m_iNumIndices = (m_GridSize - 1) * (m_GridSize - 1) * 2 * 3;
	m_indexStride = 4; // 2바이트랑 4바이트 중 택1
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Vertex Buffer//////////////////////

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;

	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXPOS* vertices = new VTXPOS[m_numVertices];

	// 버택스 갯수는 짝수 // 정사각형 가정
	_uint* pixels = new _uint[m_numVertices];

	float halfSize = m_gridSize * 0.5f;

	m_vtxPos = new _float3[m_numVertices];

	for (_uint z = 0; z < m_gridSize+1; ++z)
	{
		for (_uint x = 0; x < m_gridSize+1; ++x)
		{
			_uint index = z * (m_gridSize + 1) + x;

			vertices[index].position = _float3(x - halfSize, 0.0f, z - halfSize);

			m_vtxPos[index] = vertices[index].position;

		}

	}

	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;
	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;


	//Index Buffer/////////////////////////////


	vector<_uint> centerIndices;
	centerIndices.reserve(m_gridSize * m_gridSize * 6);
	for (size_t i = 0; i < m_gridSize ; i++)
	{
		for (size_t j = 0; j < m_gridSize; j++)
		{
			size_t index = i * (m_gridSize + 1) + j;

			_uint indices[4] = {
				index + (m_gridSize+1),
				index + (m_gridSize + 1) + 1,
				index + 1,
				index
			};

			_vector		sour, dest, normal;

			centerIndices.push_back(indices[0]);
			centerIndices.push_back(indices[1]);
			centerIndices.push_back(indices[2]);

			centerIndices.push_back(indices[0]);
			centerIndices.push_back(indices[2]);
			centerIndices.push_back(indices[3]);
		}

	}
	m_numCenterIndices = centerIndices.size();

	//////////////////////////////////////////

	vector<_uint> ringIndices;
	centerIndices.reserve(m_gridSize * m_gridSize * 6);

	int holeStart = m_gridSize / 4; // 1/4 지점
	int holeEnd = (m_gridSize * 3) / 4; // 3/4 지점
	for (size_t z = 0; m_numLevel > 1 && z < m_gridSize; z++)
	{
		for (size_t x = 0; x < m_gridSize ; x++)
		{
			if(x >= holeStart && x < holeEnd&& z >= holeStart && z < holeEnd)
			{
					continue;
			}

			size_t index = z * (m_gridSize+1) + x;

			_uint indices[4] = {
				index + (m_gridSize + 1),
				index + (m_gridSize + 1) + 1,
				index + 1,
				index
			};

			_vector		sour, dest, normal;

			ringIndices.push_back(indices[0]);
			ringIndices.push_back(indices[1]);
			ringIndices.push_back(indices[2]);

			ringIndices.push_back(indices[0]);
			ringIndices.push_back(indices[2]);
			ringIndices.push_back(indices[3]);
			
		}
	}

	m_numRingIndices = ringIndices.size();

	vector<UINT> mergedIndices;
	mergedIndices.insert(mergedIndices.end(), centerIndices.begin(), centerIndices.end());
	mergedIndices.insert(mergedIndices.end(), ringIndices.begin(), ringIndices.end());

	m_numIndices = mergedIndices.size();

	D3D11_BUFFER_DESC	indexBufferDesc{};
	indexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = m_indexStride;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = mergedIndices.data();
	if (FAILED(m_device->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_IB)))
		return E_FAIL;

	//////정리///////
	Safe_Delete_Array(pixels);
	Safe_Delete_Array(vertices);
	ringIndices.clear();
	centerIndices.clear();


	return S_OK;

}

HRESULT CVIBuffer_Sea::Initialize(void* arg)
{
	return S_OK;

}

HRESULT CVIBuffer_Sea::Bind_Resources()
{
	ID3D11Buffer* vertexBuffers[] = {
		m_VB.Get(),
		// m_pVBInstance, 
	};

	_uint			vertexStrides[] = {
		m_vertexStride,

	};

	_uint			offsets[] = {
		0
	};

	m_context->IASetVertexBuffers(0, m_numVertexBuffers, vertexBuffers, vertexStrides, offsets);
	m_context->IASetIndexBuffer(m_IB.Get(), 2 == m_indexStride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_context->IASetPrimitiveTopology(m_primitiveType);
	
	// 이 이후에 쉐이더랑 얘기 해서 버퍼의 위치를 화정

	return S_OK;


}

HRESULT CVIBuffer_Sea::Render(shared_ptr<CShader> shaderCom)
{
	const int useLOD = m_numLevel > 1 ? 1 : 0;
	if (FAILED(shaderCom->Bind_RawValue("g_UseLOD", &useLOD, sizeof(useLOD))))
		return E_FAIL;
	for(_uint i=0; i < m_numLevel; i++ )
	{
		float currentScale = pow(2.f, i); // 쉐이더에서는 float로

		shaderCom->Bind_RawValue("g_Scale", &currentScale, sizeof(float));

		shaderCom->Begin(0);

		if(i ==0 )
		{
			m_context->DrawIndexed(m_numCenterIndices, 0, 0);
			
		}
		else
		{
			m_context->DrawIndexed(m_numRingIndices, m_numCenterIndices, 0);

		}


	}


	return S_OK;
}

_float3 CVIBuffer_Sea::Snapping(float posY)
{
	_float3 result = {};
	const _float4* camPos = m_gameInstance.lock()->Get_CamPositon();

	constexpr float snapCellSize = 8.f;
	result.x = floor(camPos->x / snapCellSize) * snapCellSize;
	result.z = floor(camPos->z / snapCellSize) * snapCellSize;
	result.y = posY;

	return result;
}

shared_ptr<CVIBuffer_Sea> CVIBuffer_Sea::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, _uint numVerticesX, _uint numVerticesZ/* const _tchar* pHeightMapFilePath*/)
{
	shared_ptr<CVIBuffer_Sea> instance(new CVIBuffer_Sea(device, context, numVerticesX, numVerticesZ), [](CVIBuffer_Sea* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Sea");
	}
	return instance;
}

shared_ptr<CComponent> CVIBuffer_Sea::Clone(void* arg)
{
	shared_ptr<CVIBuffer_Sea> instance(new CVIBuffer_Sea(*this), [](CVIBuffer_Sea* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Sea");
	}
	return instance;
}

void CVIBuffer_Sea::Free()
{

	__super::Free();
	delete[] m_vtxPos;

}
