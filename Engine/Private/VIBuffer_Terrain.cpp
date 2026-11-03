#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CVIBuffer(pDevice, pContext)
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

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
 {

	wstring strPath = pHeightMapFilePath;
	wstring strExt = strPath.substr(strPath.find_last_of(L".") + 1);

	_ulong dwByte = {};
	HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	unsigned short* pRawPixels = nullptr; // RAW용 (16bit)
	_uint* pBmpPixels = nullptr;         // BMP용 (32bit)
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	if (strExt == L"raw" || strExt == L"RAW")
	{
		//헤더 없음 1500x1500 으로 뽑음
		m_numVerticesX = 1500;
		m_numVerticesZ = 1500;
		m_numVertices = m_numVerticesX * m_numVerticesZ;

		pRawPixels = new unsigned short[m_numVertices];
		ReadFile(hFile, pRawPixels, sizeof(unsigned short) * m_numVertices, &dwByte, nullptr);

	}
	if(strExt == L"bmp"|| strExt == L"BMP")
	{
		BITMAPFILEHEADER fh = {};
		ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);

		BITMAPINFOHEADER ih = {};
		ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

		m_numVerticesX = ih.biWidth;
		m_numVerticesZ = ih.biHeight;
		m_numVertices = m_numVerticesX * m_numVerticesZ;

		// 해당 비트맵 파일이 rgba 8 비트씩 32비트 픽셀하나당 이므로 _uint
		pBmpPixels = new _uint[m_numVertices];
		ReadFile(hFile, pBmpPixels, sizeof(_uint) * m_numVertices, &dwByte, nullptr);
	}
//----------하이트맵 읽기 끝

	m_numVertexBuffers = 1; // 버택스 버퍼가 몇갠지 사각형그리는 거니까 1개
	//m_iNumVertices =
	m_vertexStride = sizeof(VTXNORTEX);
	m_numIndices = (m_numVerticesX-1)* (m_numVerticesZ-1)*2*3;
	m_indexStride = 4; // 2바이트랑 4바이트 중 택1
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXNORTEX* pVertices = new VTXNORTEX[m_numVertices];
	m_vtxPos = new _float3[m_numVertices];

	for (size_t i = 0; i < m_numVerticesZ; i++)
	{
		for (size_t j = 0; j < m_numVerticesX; j++)
		{
			size_t iIndex = i * m_numVerticesX + j;

			float fHeight = 0.f;

			float fHeightRatio = 0.0f; // 0.0 ~ 1.0 비율만 담을 임시 변수

			if (pRawPixels)
			{
				fHeightRatio = pRawPixels[iIndex] / 65535.0f;
			}
			m_vecHeightData.push_back(fHeightRatio);

			if (pRawPixels) // RAW일 때 높이 계산 (16비트)
				fHeight = (pRawPixels[iIndex] / 65535.0f) * 99.8f;
			else if (pBmpPixels) // BMP일 때 높이 계산 (8비트)
				fHeight = (pBmpPixels[iIndex] & 0x000000ff) / 10.f;

			
			//정점 사이의 인터벌은 무조건 1
			pVertices[iIndex].position = _float3(j, fHeight, i);
			pVertices[iIndex].normal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].texcoord = _float2(j / (m_numVerticesX - 1.f), i / (m_numVerticesZ - 1.f));
		
			m_vtxPos[iIndex] = pVertices[iIndex].position;
		
		}

	}


	//Index Buffer

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_indexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_numIndices];
	_uint	iNumIndices = { 0 };

	for (size_t i = 0; i < m_numVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_numVerticesX - 1; j++)
		{
			size_t iIndex = i * m_numVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_numVerticesX,
				iIndex + m_numVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].position) - XMLoadFloat3(&pVertices[iIndices[0]].position);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].position) - XMLoadFloat3(&pVertices[iIndices[1]].position);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].normal,
				XMLoadFloat3(&pVertices[iIndices[0]].normal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].normal,
				XMLoadFloat3(&pVertices[iIndices[1]].normal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].normal,
				XMLoadFloat3(&pVertices[iIndices[2]].normal) + vNormal);


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].position) - XMLoadFloat3(&pVertices[iIndices[0]].position);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].position) - XMLoadFloat3(&pVertices[iIndices[2]].position);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));


			XMStoreFloat3(&pVertices[iIndices[0]].normal,
				XMLoadFloat3(&pVertices[iIndices[0]].normal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].normal,
				XMLoadFloat3(&pVertices[iIndices[2]].normal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].normal,
				XMLoadFloat3(&pVertices[iIndices[3]].normal) + vNormal);


		}

	}

	for (size_t i = 0; i < m_numVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].normal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].normal)));
	}

	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;
	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
		return E_FAIL;

	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;
	if (FAILED(m_device->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_IB)))
		return E_FAIL;

	if(pRawPixels)
	Safe_Delete_Array(pRawPixels);
	if (pBmpPixels)
	Safe_Delete_Array(pBmpPixels);
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	CloseHandle(hFile);

	
	return S_OK;

}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;

}

shared_ptr<CVIBuffer_Terrain> CVIBuffer_Terrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pHeightMapFilePath)
{
	shared_ptr<CVIBuffer_Terrain> pInstance(new CVIBuffer_Terrain(pDevice, pContext), [](CVIBuffer_Terrain* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Terrain::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_Terrain> pInstance ( new CVIBuffer_Terrain(*this), [](CVIBuffer_Terrain* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{

	__super::Free();
	delete[] m_vtxPos;
	m_vecHeightData.clear();

}
