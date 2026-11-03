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

HRESULT CAssimp_Mesh::Initialize_Prototype(EModelType eType, const aiMesh* pAIMesh, shared_ptr<CAssimp_Model> pModel, _fmatrix PreTransformMatrix)
{
	m_materialIndex = pAIMesh->mMaterialIndex;
	
	m_numVertexBuffers = 1; 
	m_numVertices = pAIMesh->mNumVertices;
	//m_iVertexStride = sizeof(VTXMESH);
	m_numIndices = pAIMesh->mNumFaces * 3;// mNumFaces 삼각현이든 사각형이든 한 면의갯수 우리는 삼각형이니까 *3 하면 인덱스 
	m_indexStride = 4; // 크게 4바이트로 잡음
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//vertex Buffer
	HRESULT hr = EModelType::NONANIM == eType ? Ready_VertexBuffer_For_NonAnim(pAIMesh, PreTransformMatrix)
		: Ready_VertexBuffer_For_Anim(pAIMesh, pModel);

	if (FAILED(hr))
		return E_FAIL;

	//Index Buffer

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_indexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	_uint	iNumIndices = { 0 };

	_uint* pIndices = new _uint[m_numIndices];

	for (_uint i = 0; i < pAIMesh->mNumFaces; i++)
	{
		
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}

	
	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_device->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_IB)))
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
	ZeroMemory(m_boneMatrices, sizeof(_float4x4) * 512);

	for(size_t i =0; i<m_numBones; i++)
	{
		XMStoreFloat4x4(&m_boneMatrices[i], XMLoadFloat4x4(&m_offsetMatrices[i] )*
			XMLoadFloat4x4(bones[m_boneIndices[i]]->Get_m_CombinedTransformationMatrixPtr()));
	}
	
		return shader->Bind_Matrices(pConstantName, m_boneMatrices, m_numBones);
}


HRESULT CAssimp_Mesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_vertexStride = sizeof(VTXMESH);

	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXMESH* pVertices = new VTXMESH[m_numVertices];

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&pVertices[i].position, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].position,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].position), PreTransformMatrix));

		memcpy(&pVertices[i].normal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].normal,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].normal), PreTransformMatrix));

		memcpy(&pVertices[i].tangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].tangent,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].tangent), PreTransformMatrix));

		memcpy(&pVertices[i].texcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		
		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}
	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CAssimp_Mesh::Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, shared_ptr<CAssimp_Model> pModel)
{
	/*struct WeightData
	{
		int boneIndex;
		float weight;
	};

	vector<vector<WeightData>> tempWeights(m_iNumVertices);*/


	m_vertexStride = sizeof(VTXANIMMESH);

	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_numVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_numVertices);

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&pVertices[i].position, &pAIMesh->mVertices[i], sizeof(_float3));
		
		memcpy(&pVertices[i].normal, &pAIMesh->mNormals[i], sizeof(_float3));
		
		memcpy(&pVertices[i].tangent, &pAIMesh->mTangents[i], sizeof(_float3));

		memcpy(&pVertices[i].texcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}

		m_numBones = pAIMesh->mNumBones;

	for (size_t i = 0; i < m_numBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_int iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);//(이건 전체 기준 본 인덱스)
		if (-1 == iBoneIndex)
			return E_FAIL;

		m_boneIndices.push_back(iBoneIndex); // 여기 넣은 순서가 메쉬기준 본 인댁스 임 

		// set offset once in initialize 
		_float4x4 OffsetMatrix = {};

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_offsetMatrices.push_back(OffsetMatrix);


		//pAIBone->mNumWeights 이 뼈가 몇개의 정점에 영향을 주는가
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight AIVertexWeight = pAIBone->mWeights[j];

			if(0== pVertices[AIVertexWeight.mVertexId].blendWeight.x) //pVertices[AIVertexWeight.mVertexId] 에 이미적혀 있는지 없는지 확인용
			{
				pVertices[AIVertexWeight.mVertexId].blendIndex.x = i;  // 뼈의 인댁스 (메쉬 기준)
				pVertices[AIVertexWeight.mVertexId].blendWeight.x = AIVertexWeight.mWeight;
			}
			else if(0 == pVertices[AIVertexWeight.mVertexId].blendWeight.y)
			{
				pVertices[AIVertexWeight.mVertexId].blendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].blendWeight.y = AIVertexWeight.mWeight;
			}
			else if (0 == pVertices[AIVertexWeight.mVertexId].blendWeight.z)
			{
				pVertices[AIVertexWeight.mVertexId].blendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].blendWeight.z = AIVertexWeight.mWeight;
			}
			else if (0 == pVertices[AIVertexWeight.mVertexId].blendWeight.w)
			{
				pVertices[AIVertexWeight.mVertexId].blendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].blendWeight.w = AIVertexWeight.mWeight;
			}
			//int vtxId = AIVertexWeight.mVertexId;

	
			//tempWeights[vtxId].push_back({ (int)i, AIVertexWeight.mWeight });
		}
	}
	//for (size_t i = 0; i < m_iNumVertices; i++)
	//{
	//	auto& weights = tempWeights[i];

	//	// 큰 weight 순으로 정렬
	//	sort(weights.begin(), weights.end(), [](auto& a, auto& b)
	//		{
	//			return a.weight > b.weight;
	//		});

	//	// 상위 4개만 사용
	//	for (int k = 0; k < 4 && k < weights.size(); k++)
	//	{
	//		switch (k)
	//		{
	//		case 0:
	//			pVertices[i].vBlendIndex.x = weights[k].boneIndex;
	//			pVertices[i].vBlendWeight.x = weights[k].weight;
	//			break;
	//		case 1:
	//			pVertices[i].vBlendIndex.y = weights[k].boneIndex;
	//			pVertices[i].vBlendWeight.y = weights[k].weight;
	//			break;
	//		case 2:
	//			pVertices[i].vBlendIndex.z = weights[k].boneIndex;
	//			pVertices[i].vBlendWeight.z = weights[k].weight;
	//			break;
	//		case 3:
	//			pVertices[i].vBlendIndex.w = weights[k].boneIndex;
	//			pVertices[i].vBlendWeight.w = weights[k].weight;
	//			break;
	//		}
	//	}
	//}

	//for (size_t i = 0; i < m_iNumVertices; i++)
	//{
	//	float sum =
	//		pVertices[i].vBlendWeight.x +
	//		pVertices[i].vBlendWeight.y +
	//		pVertices[i].vBlendWeight.z +
	//		pVertices[i].vBlendWeight.w;

	//	if (sum > 0.f)
	//	{
	//		pVertices[i].vBlendWeight.x /= sum;
	//		pVertices[i].vBlendWeight.y /= sum;
	//		pVertices[i].vBlendWeight.z /= sum;
	//		pVertices[i].vBlendWeight.w /= sum;
	//	}
	//}


	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;

}

shared_ptr<CAssimp_Mesh> CAssimp_Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, EModelType eType, const aiMesh* pAIMesh, shared_ptr<CAssimp_Model> pModel, _fmatrix PreTransformMatrix)
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
