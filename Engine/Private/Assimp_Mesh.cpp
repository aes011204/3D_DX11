#include "Assimp_Mesh.h"
#include "Assimp_Model.h"
#include "Shader.h"

CAssimp_Mesh::CAssimp_Mesh(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    :CVIBuffer(device, context)
{
}

CAssimp_Mesh::CAssimp_Mesh(const CAssimp_Mesh& Prototype)
    : CVIBuffer(Prototype)
{
}

HRESULT CAssimp_Mesh::Initialize_Prototype(EModelType type, const aiMesh* srcAIMesh, shared_ptr<CAssimp_Model> model, _fmatrix preTransformMatrix)
{
	m_materialIndex = srcAIMesh->mMaterialIndex;
	
	m_numVertexBuffers = 1; 
	m_numVertices = srcAIMesh->mNumVertices;
	//m_iVertexStride = sizeof(VTXMESH);
	m_numIndices = srcAIMesh->mNumFaces * 3;// mNumFaces 삼각현이든 사각형이든 한 면의갯수 우리는 삼각형이니까 *3 하면 인덱스 
	m_indexStride = 4; // 크게 4바이트로 잡음
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//vertex Buffer
	HRESULT hr = EModelType::NONANIM == type ? Ready_VertexBuffer_For_NonAnim(srcAIMesh, preTransformMatrix)
		: Ready_VertexBuffer_For_Anim(srcAIMesh, model);

	if (FAILED(hr))
		return E_FAIL;

	//Index Buffer

	D3D11_BUFFER_DESC	indexBufferDesc{};
	indexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = m_indexStride;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	_uint	numIndices = { 0 };

	_uint* indices = new _uint[m_numIndices];

	for (_uint i = 0; i < srcAIMesh->mNumFaces; i++)
	{
		
		aiFace srcAIFace = srcAIMesh->mFaces[i];

		indices[numIndices++] = srcAIFace.mIndices[0];
		indices[numIndices++] = srcAIFace.mIndices[1];
		indices[numIndices++] = srcAIFace.mIndices[2];
	}

	
	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = indices;

	if (FAILED(m_device->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_IB)))
		return E_FAIL;



	Safe_Delete_Array(indices);


	return S_OK;
}

HRESULT CAssimp_Mesh::Initialize(void* arg)
{
    return S_OK;
}

HRESULT CAssimp_Mesh::Bind_BoneMatrices(shared_ptr<CShader> shader, const _char* constantName,
	const vector<shared_ptr<CAssimp_Bone>>& bones)
{
	ZeroMemory(m_boneMatrices, sizeof(_float4x4) * 512);

	for(size_t i =0; i<m_numBones; i++)
	{
		XMStoreFloat4x4(&m_boneMatrices[i], XMLoadFloat4x4(&m_offsetMatrices[i] )*
			XMLoadFloat4x4(bones[m_boneIndices[i]]->Get_CombinedTransformationMatrixPtr()));
	}
	
		return shader->Bind_Matrices(constantName, m_boneMatrices, m_numBones);
}


HRESULT CAssimp_Mesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* srcAIMesh, _fmatrix preTransformMatrix)
{
	m_vertexStride = sizeof(VTXMESH);

	// Vertex Buffer

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;

	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXMESH* vertices = new VTXMESH[m_numVertices];

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&vertices[i].position, &srcAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&vertices[i].position,
			XMVector3TransformCoord(XMLoadFloat3(&vertices[i].position), preTransformMatrix));

		memcpy(&vertices[i].normal, &srcAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&vertices[i].normal,
			XMVector3TransformCoord(XMLoadFloat3(&vertices[i].normal), preTransformMatrix));

		memcpy(&vertices[i].tangent, &srcAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&vertices[i].tangent,
			XMVector3TransformCoord(XMLoadFloat3(&vertices[i].tangent), preTransformMatrix));

		memcpy(&vertices[i].texcoord, &srcAIMesh->mTextureCoords[0][i], sizeof(_float2));

		
		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}
	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;

	Safe_Delete_Array(vertices);

	return S_OK;
}

HRESULT CAssimp_Mesh::Ready_VertexBuffer_For_Anim(const aiMesh* srcAIMesh, shared_ptr<CAssimp_Model> model)
{
	/*struct WeightData
	{
		int boneIndex;
		float weight;
	};

	vector<vector<WeightData>> tempWeights(m_iNumVertices);*/


	m_vertexStride = sizeof(VTXANIMMESH);

	// Vertex Buffer

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;

	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXANIMMESH* vertices = new VTXANIMMESH[m_numVertices];
	ZeroMemory(vertices, sizeof(VTXANIMMESH) * m_numVertices);

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&vertices[i].position, &srcAIMesh->mVertices[i], sizeof(_float3));
		
		memcpy(&vertices[i].normal, &srcAIMesh->mNormals[i], sizeof(_float3));
		
		memcpy(&vertices[i].tangent, &srcAIMesh->mTangents[i], sizeof(_float3));

		memcpy(&vertices[i].texcoord, &srcAIMesh->mTextureCoords[0][i], sizeof(_float2));

		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}

		m_numBones = srcAIMesh->mNumBones;

	for (size_t i = 0; i < m_numBones; i++)
	{
		aiBone* srcAIBone = srcAIMesh->mBones[i];

		_int boneIndex = model->Get_BoneIndex(srcAIBone->mName.data);//(이건 전체 기준 본 인덱스)
		if (-1 == boneIndex)
			return E_FAIL;

		m_boneIndices.push_back(boneIndex); // 여기 넣은 순서가 메쉬기준 본 인댁스 임 

		// set offset once in initialize 
		_float4x4 offsetMatrix = {};

		memcpy(&offsetMatrix, &srcAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&offsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&offsetMatrix)));

		m_offsetMatrices.push_back(offsetMatrix);


		//pAIBone->mNumWeights 이 뼈가 몇개의 정점에 영향을 주는가
		for (size_t j = 0; j < srcAIBone->mNumWeights; j++)
		{
			aiVertexWeight srcAIVertexWeight = srcAIBone->mWeights[j];

			if(0== vertices[srcAIVertexWeight.mVertexId].blendWeight.x) //pVertices[AIVertexWeight.mVertexId] 에 이미적혀 있는지 없는지 확인용
			{
				vertices[srcAIVertexWeight.mVertexId].blendIndex.x = i;  // 뼈의 인댁스 (메쉬 기준)
				vertices[srcAIVertexWeight.mVertexId].blendWeight.x = srcAIVertexWeight.mWeight;
			}
			else if(0 == vertices[srcAIVertexWeight.mVertexId].blendWeight.y)
			{
				vertices[srcAIVertexWeight.mVertexId].blendIndex.y = i;
				vertices[srcAIVertexWeight.mVertexId].blendWeight.y = srcAIVertexWeight.mWeight;
			}
			else if (0 == vertices[srcAIVertexWeight.mVertexId].blendWeight.z)
			{
				vertices[srcAIVertexWeight.mVertexId].blendIndex.z = i;
				vertices[srcAIVertexWeight.mVertexId].blendWeight.z = srcAIVertexWeight.mWeight;
			}
			else if (0 == vertices[srcAIVertexWeight.mVertexId].blendWeight.w)
			{
				vertices[srcAIVertexWeight.mVertexId].blendIndex.w = i;
				vertices[srcAIVertexWeight.mVertexId].blendWeight.w = srcAIVertexWeight.mWeight;
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


	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;

	Safe_Delete_Array(vertices);

	return S_OK;

}

shared_ptr<CAssimp_Mesh> CAssimp_Mesh::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, EModelType type, const aiMesh* srcAIMesh, shared_ptr<CAssimp_Model> model, _fmatrix preTransformMatrix)
{
    shared_ptr<CAssimp_Mesh> instance(new CAssimp_Mesh(device, context), [](CAssimp_Mesh* p) {p->Free(); delete p;});

    if (FAILED(instance->Initialize_Prototype(type ,srcAIMesh, model, preTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CAssimp_Mesh");
    }
    return instance;

}

shared_ptr<CComponent> CAssimp_Mesh::Clone(void* arg)
{

    shared_ptr<CComponent> instance(new CAssimp_Mesh(*this), [](CAssimp_Mesh* p) {p->Free(); delete p;});

    if (FAILED(instance->Initialize(arg)))
    {
        MSG_BOX("Failed to Cloned : CAssimp_Mesh");
    }
    return instance;
}


void CAssimp_Mesh::Free()
{
    __super::Free();
}
