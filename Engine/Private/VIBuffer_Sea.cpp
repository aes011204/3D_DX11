#include "VIBuffer_Sea.h"

#include "GameInstance.h"
#include "Shader.h"

CVIBuffer_Sea::CVIBuffer_Sea(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint gridSize, _uint NumLevel)
	:CVIBuffer(pDevice, pContext) ,m_GridSize(gridSize), m_NumLevel(NumLevel)
{
}

CVIBuffer_Sea::CVIBuffer_Sea(const CVIBuffer_Sea& Prototype)
	: CVIBuffer(Prototype),
	m_NumLevel(Prototype.m_NumLevel), m_GridSize(Prototype.m_GridSize),
m_iNumRingIndices(Prototype.m_iNumRingIndices), m_iNumCenterIndices(Prototype.m_iNumCenterIndices)
{
	m_VtxPos = new _float3[m_iNumVertices];
	memcpy(m_VtxPos, Prototype.m_VtxPos, sizeof(_float3) * m_iNumVertices);
}

HRESULT CVIBuffer_Sea::Initialize_Prototype(/*const _tchar* pHeightMapFilePath*/)
{
	// 무조건 4의 배수여야 함
	if (m_GridSize % 4 != 0)
	{
		MSG_BOX("wrong size");
		return E_FAIL;
	}

	m_iNumVertexBuffers = 1; // 버택스 버퍼가 몇갠지 사각형그리는 거니까 1개
	m_iNumVertices = (m_GridSize+1) * (m_GridSize+1);
	m_iVertexStride = sizeof(VTXPOS);
	//m_iNumIndices = (m_GridSize - 1) * (m_GridSize - 1) * 2 * 3;
	m_iIndexStride = 4; // 2바이트랑 4바이트 중 택1
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Vertex Buffer//////////////////////

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];

	// 버택스 갯수는 짝수 // 정사각형 가정
	_uint* pPixels = new _uint[m_iNumVertices];

	float halfSize = m_GridSize * 0.5f;

	m_VtxPos = new _float3[m_iNumVertices];

	for (_uint z = 0; z < m_GridSize+1; ++z)
	{
		for (_uint x = 0; x < m_GridSize+1; ++x)
		{
			_uint iIndex = z * (m_GridSize + 1) + x;

			pVertices[iIndex].vPosition = _float3(x - halfSize, 0.0f, z - halfSize);

			m_VtxPos[iIndex] = pVertices[iIndex].vPosition;

		}

	}

	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;
	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;


	//Index Buffer/////////////////////////////


	vector<_uint> pCenterIndices;
	pCenterIndices.reserve(m_GridSize * m_GridSize * 6);
	for (size_t i = 0; i < m_GridSize ; i++)
	{
		for (size_t j = 0; j < m_GridSize; j++)
		{
			size_t iIndex = i * (m_GridSize + 1) + j;

			_uint iIndices[4] = {
				iIndex + (m_GridSize+1),
				iIndex + (m_GridSize + 1) + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;

			pCenterIndices.push_back(iIndices[0]);
			pCenterIndices.push_back(iIndices[1]);
			pCenterIndices.push_back(iIndices[2]);

			pCenterIndices.push_back(iIndices[0]);
			pCenterIndices.push_back(iIndices[2]);
			pCenterIndices.push_back(iIndices[3]);
		}

	}
	m_iNumCenterIndices = pCenterIndices.size();

	//////////////////////////////////////////

	vector<_uint> pRingIndices;
	pCenterIndices.reserve(m_GridSize * m_GridSize * 6);

	int holeStart = m_GridSize / 4; // 1/4 지점
	int holeEnd = (m_GridSize * 3) / 4; // 3/4 지점
	for (size_t z = 0; z < m_GridSize ; z++)
	{
		for (size_t x = 0; x < m_GridSize ; x++)
		{
			if(x >= holeStart && x < holeEnd&& z >= holeStart && z < holeEnd)
			{
					continue;
			}

			size_t iIndex = z * (m_GridSize+1) + x;

			_uint iIndices[4] = {
				iIndex + (m_GridSize + 1),
				iIndex + (m_GridSize + 1) + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;

			pRingIndices.push_back(iIndices[0]);
			pRingIndices.push_back(iIndices[1]);
			pRingIndices.push_back(iIndices[2]);

			pRingIndices.push_back(iIndices[0]);
			pRingIndices.push_back(iIndices[2]);
			pRingIndices.push_back(iIndices[3]);
			
		}
	}

	m_iNumRingIndices = pRingIndices.size();

	vector<UINT> mergedIndices;
	mergedIndices.insert(mergedIndices.end(), pCenterIndices.begin(), pCenterIndices.end());
	mergedIndices.insert(mergedIndices.end(), pRingIndices.begin(), pRingIndices.end());

	m_iNumIndices = mergedIndices.size();

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_iIndexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = mergedIndices.data();
	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
		return E_FAIL;

	//////정리///////
	Safe_Delete_Array(pPixels);
	Safe_Delete_Array(pVertices);
	pRingIndices.clear();
	pCenterIndices.clear();


	return S_OK;

}

HRESULT CVIBuffer_Sea::Initialize(void* pArg)
{
	return S_OK;

}

HRESULT CVIBuffer_Sea::Bind_Resources()
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

HRESULT CVIBuffer_Sea::Render(shared_ptr<CShader> ShaderCom)
{
	for(_uint i=0; i < m_NumLevel; i++ )
	{
		float currentScale = pow(2.f, i); // 쉐이더에서는 float로

		ShaderCom->Bind_RawValue("g_Scale", &currentScale, sizeof(float));

		ShaderCom->Begin(0);

		if(i ==0 )
		{
			m_pContext->DrawIndexed(m_iNumCenterIndices, 0, 0);
			
		}
		else
		{
			m_pContext->DrawIndexed(m_iNumRingIndices, m_iNumCenterIndices, 0);

		}


	}


	return S_OK;
}

_float3 CVIBuffer_Sea::Snaping(float PosY)
{
	_float3 result = {};
	const _float4* camPos = m_pGameInstance.lock()->Get_CamPositon();

	result.x = floor(camPos->x / 1.f) * 1.f;
	result.z = floor(camPos->z / 1.f) * 1.f;
	result.y = PosY;

	return result;
}

shared_ptr<CVIBuffer_Sea> CVIBuffer_Sea::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint NumVerticesX, _uint NumVerticesZ/* const _tchar* pHeightMapFilePath*/)
{
	shared_ptr<CVIBuffer_Sea> pInstance(new CVIBuffer_Sea(pDevice, pContext, NumVerticesX, NumVerticesZ), [](CVIBuffer_Sea* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Sea");
	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Sea::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_Sea> pInstance(new CVIBuffer_Sea(*this), [](CVIBuffer_Sea* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Sea");
	}
	return pInstance;
}

void CVIBuffer_Sea::Free()
{

	__super::Free();
	delete[] m_VtxPos;

}
