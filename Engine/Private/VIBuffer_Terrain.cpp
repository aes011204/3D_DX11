#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CVIBuffer(device, context)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer(Prototype),
	m_numVerticesX{Prototype.m_numVerticesX},
	m_numVerticesZ{Prototype.m_numVerticesZ}
{
	m_vtxPos = new _float3[m_numVertices];
	memcpy(m_vtxPos, Prototype.m_vtxPos, sizeof(_float3) * m_numVertices);
	m_vecHeightData = Prototype.m_vecHeightData;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* heightMapFilePath)
 {

	wstring path = heightMapFilePath;
	wstring ext = path.substr(path.find_last_of(L".") + 1);

	_ulong byte = {};
	HANDLE hFile = CreateFile(heightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	unsigned short* rawPixels = nullptr; // RAW용 (16bit)
	_uint* bmpPixels = nullptr;         // BMP용 (32bit)
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	if (ext == L"raw" || ext == L"RAW")
	{
		//헤더 없음 1500x1500 으로 뽑음
		m_numVerticesX = 1500;
		m_numVerticesZ = 1500;
		m_numVertices = m_numVerticesX * m_numVerticesZ;

		rawPixels = new unsigned short[m_numVertices];
		ReadFile(hFile, rawPixels, sizeof(unsigned short) * m_numVertices, &byte, nullptr);

	}
	if(ext == L"bmp"|| ext == L"BMP")
	{
		BITMAPFILEHEADER fh = {};
		ReadFile(hFile, &fh, sizeof(fh), &byte, nullptr);

		BITMAPINFOHEADER ih = {};
		ReadFile(hFile, &ih, sizeof(ih), &byte, nullptr);

		m_numVerticesX = ih.biWidth;
		m_numVerticesZ = ih.biHeight;
		m_numVertices = m_numVerticesX * m_numVerticesZ;

		// 해당 비트맵 파일이 rgba 8 비트씩 32비트 픽셀하나당 이므로 _uint
		bmpPixels = new _uint[m_numVertices];
		ReadFile(hFile, bmpPixels, sizeof(_uint) * m_numVertices, &byte, nullptr);
	}
//----------하이트맵 읽기 끝

	m_numVertexBuffers = 1; // 버택스 버퍼가 몇갠지 사각형그리는 거니까 1개
	//m_iNumVertices =
	m_vertexStride = sizeof(VTXNORTEX);
	m_numIndices = (m_numVerticesX-1)* (m_numVerticesZ-1)*2*3;
	m_indexStride = 4; // 2바이트랑 4바이트 중 택1
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Vertex Buffer

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;

	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXNORTEX* vertices = new VTXNORTEX[m_numVertices];
	m_vtxPos = new _float3[m_numVertices];

	for (size_t i = 0; i < m_numVerticesZ; i++)
	{
		for (size_t j = 0; j < m_numVerticesX; j++)
		{
			size_t index = i * m_numVerticesX + j;

			float height = 0.f;

			float heightRatio = 0.0f; // 0.0 ~ 1.0 비율만 담을 임시 변수

			if (rawPixels)
			{
				heightRatio = rawPixels[index] / 65535.0f;
			}
			m_vecHeightData.push_back(heightRatio);

			if (rawPixels) // RAW일 때 높이 계산 (16비트)
				height = (rawPixels[index] / 65535.0f) * 99.8f;
			else if (bmpPixels) // BMP일 때 높이 계산 (8비트)
				height = (bmpPixels[index] & 0x000000ff) / 10.f;

			
			//정점 사이의 인터벌은 무조건 1
			vertices[index].position = _float3(j, height, i);
			vertices[index].normal = _float3(0.f, 0.f, 0.f);
			vertices[index].texcoord = _float2(j / (m_numVerticesX - 1.f), i / (m_numVerticesZ - 1.f));
		
			m_vtxPos[index] = vertices[index].position;
		
		}

	}


	//Index Buffer

	D3D11_BUFFER_DESC	indexBufferDesc{};
	indexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = m_indexStride;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	_uint* indices = new _uint[m_numIndices];
	_uint	numIndices = { 0 };

	for (size_t i = 0; i < m_numVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_numVerticesX - 1; j++)
		{
			size_t index = i * m_numVerticesX + j;

			_uint quadIndices[4] = {
				index + m_numVerticesX,
				index + m_numVerticesX + 1,
				index + 1,
				index
			};

			_vector		sour, dest, normal;

			indices[numIndices++] = quadIndices[0];
			indices[numIndices++] = quadIndices[1];
			indices[numIndices++] = quadIndices[2];

			sour = XMLoadFloat3(&vertices[quadIndices[1]].position) - XMLoadFloat3(&vertices[quadIndices[0]].position);
			dest = XMLoadFloat3(&vertices[quadIndices[2]].position) - XMLoadFloat3(&vertices[quadIndices[1]].position);
			normal = XMVector3Normalize(XMVector3Cross(sour, dest));

			XMStoreFloat3(&vertices[quadIndices[0]].normal,
				XMLoadFloat3(&vertices[quadIndices[0]].normal) + normal);
			XMStoreFloat3(&vertices[quadIndices[1]].normal,
				XMLoadFloat3(&vertices[quadIndices[1]].normal) + normal);
			XMStoreFloat3(&vertices[quadIndices[2]].normal,
				XMLoadFloat3(&vertices[quadIndices[2]].normal) + normal);


			indices[numIndices++] = quadIndices[0];
			indices[numIndices++] = quadIndices[2];
			indices[numIndices++] = quadIndices[3];

			sour = XMLoadFloat3(&vertices[quadIndices[2]].position) - XMLoadFloat3(&vertices[quadIndices[0]].position);
			dest = XMLoadFloat3(&vertices[quadIndices[3]].position) - XMLoadFloat3(&vertices[quadIndices[2]].position);
			normal = XMVector3Normalize(XMVector3Cross(sour, dest));


			XMStoreFloat3(&vertices[quadIndices[0]].normal,
				XMLoadFloat3(&vertices[quadIndices[0]].normal) + normal);
			XMStoreFloat3(&vertices[quadIndices[2]].normal,
				XMLoadFloat3(&vertices[quadIndices[2]].normal) + normal);
			XMStoreFloat3(&vertices[quadIndices[3]].normal,
				XMLoadFloat3(&vertices[quadIndices[3]].normal) + normal);


		}

	}

	for (size_t i = 0; i < m_numVertices; i++)
	{
		XMStoreFloat3(&vertices[i].normal, XMVector3Normalize(XMLoadFloat3(&vertices[i].normal)));
	}

	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;
	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;

	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = indices;
	if (FAILED(m_device->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_IB)))
		return E_FAIL;

	if(rawPixels)
	Safe_Delete_Array(rawPixels);
	if (bmpPixels)
	Safe_Delete_Array(bmpPixels);
	Safe_Delete_Array(vertices);
	Safe_Delete_Array(indices);

	CloseHandle(hFile);

	
	return S_OK;

}

HRESULT CVIBuffer_Terrain::Initialize(void* arg)
{
	return S_OK;

}

shared_ptr<CVIBuffer_Terrain> CVIBuffer_Terrain::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _tchar* heightMapFilePath)
{
	shared_ptr<CVIBuffer_Terrain> instance(new CVIBuffer_Terrain(device, context), [](CVIBuffer_Terrain* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize_Prototype(heightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
	}
	return instance;
}

shared_ptr<CComponent> CVIBuffer_Terrain::Clone(void* arg)
{
	shared_ptr<CVIBuffer_Terrain> instance ( new CVIBuffer_Terrain(*this), [](CVIBuffer_Terrain* p) {p->Free(); delete p;});

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
	}
	return instance;
}

void CVIBuffer_Terrain::Free()
{

	__super::Free();
	delete[] m_vtxPos;
	m_vecHeightData.clear();

}
