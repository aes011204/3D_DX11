#include "Assimp_Mesh.h"
#include "Assimp_Model.h"
#include "Shader.h"

CAssimp_Mesh::CAssimp_Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice, pContext)
{
}

CAssimp_Mesh::CAssimp_Mesh(const CAssimp_Mesh& Prototype)
    : CVIBuffer(Prototype)
{
}

HRESULT CAssimp_Mesh::Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, shared_ptr<CAssimp_Model> pModel, _fmatrix PreTransformMatrix)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	
	m_iNumVertexBuffers = 1; 
	m_iNumVertices = pAIMesh->mNumVertices;
	//m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = pAIMesh->mNumFaces * 3;// mNumFaces 삼각현이든 사각형이든 한 면의갯수 우리는 삼각형이니까 *3 하면 인덱스 
	m_iIndexStride = 4; // 크게 4바이트로 잡음
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//vertex Buffer
	HRESULT hr = MODEL::NONANIM == eType ? Ready_VertexBuffer_For_NonAnim(pAIMesh, PreTransformMatrix)
		: Ready_VertexBuffer_For_Anim(pAIMesh, pModel);

	if (FAILED(hr))
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



	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CAssimp_Mesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CAssimp_Mesh::Bind_BoneMatrices(shared_ptr<CShader> shader, const _char* pConstantName,
	const vector<shared_ptr<CAssimp_Bone>>& bones)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * 512);

	for(size_t i =0; i<m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i] )*
			XMLoadFloat4x4(bones[m_BoneIndices[i]]->Get_m_CombinedTransformationMatrixPtr()));
	}
	
		return shader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}


HRESULT CAssimp_Mesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

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

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CAssimp_Mesh::Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, shared_ptr<CAssimp_Model> pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}

		m_iNumBones = pAIMesh->mNumBones;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_int iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);//(이건 전체 기준 본 인덱스)
		if (-1 == iBoneIndex)
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex); // 여기 넣은 순서가 메쉬기준 본 인댁스 임 

		// set offset once in initialize 
		_float4x4 OffsetMatrix = {};

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);


		//pAIBone->mNumWeights 이 뼈가 몇개의 정점에 영향을 주는가
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight AIVertexWeight = pAIBone->mWeights[j];

			if(0== pVertices[AIVertexWeight.mVertexId].vBlendWeight.x) //pVertices[AIVertexWeight.mVertexId] 에 이미적혀 있는지 없는지 확인용
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = i;  // 뼈의 인댁스 (메쉬 기준)
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
			}
			else if(0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
			}
			else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
			}
			else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.w)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
			}
		}
	}


	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;

}

shared_ptr<CAssimp_Mesh> CAssimp_Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, MODEL eType, const aiMesh* pAIMesh, shared_ptr<CAssimp_Model> pModel, _fmatrix PreTransformMatrix)
{
    shared_ptr<CAssimp_Mesh> pInstance(new CAssimp_Mesh(pDevice, pContext), [](CAssimp_Mesh* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize_Prototype(eType ,pAIMesh, pModel, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CAssimp_Mesh");
    }
    return pInstance;

}

shared_ptr<CComponent> CAssimp_Mesh::Clone(void* pArg)
{

    shared_ptr<CComponent> pInstance(new CAssimp_Mesh(*this), [](CAssimp_Mesh* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAssimp_Mesh");
    }
    return pInstance;
}


void CAssimp_Mesh::Free()
{
    __super::Free();
}
