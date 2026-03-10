#include "Mesh.h"

CMesh::CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& Prototype)
    : CVIBuffer(Prototype)
{
}

HRESULT CMesh::Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	
	
	m_iNumVertexBuffers = 1; 
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = pAIMesh->mNumFaces * 3;// mNumFaces 삼각현이든 사각형이든 한 면의갯수 우리는 삼각형이니까 *3 하면 인덱스 
	m_iIndexStride = 4; // 크게 4바이트로 잡음
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}
	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;


	//Index Buffer

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_iIndexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	_uint	iNumIndices = { 0 };

	_uint* pIndices = new _uint[m_iNumIndices];

	for (_uint i = 0; i < pAIMesh->mNumFaces; i++)
	{
		
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}

	
	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
		return E_FAIL;


	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

shared_ptr<CMesh> CMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    shared_ptr<CMesh> pInstance(new CMesh(pDevice, pContext), [](CMesh* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize_Prototype(pAIMesh, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CMesh");
    }
    return pInstance;

}

shared_ptr<CComponent> CMesh::Clone(void* pArg)
{

    shared_ptr<CComponent> pInstance(new CMesh(*this), [](CMesh* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMesh");
    }
    return pInstance;
}


void CMesh::Free()
{
    __super::Free();
}
