#include "Mesh.h"
#include "Model.h"
#include "Shader.h"

CMesh::CMesh(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    :CVIBuffer(device, context)
{
}

CMesh::CMesh(const CMesh& Prototype)
    : CVIBuffer(Prototype)
{
}

HRESULT CMesh::Initialize_Prototype(EModelType type,
	 ifstream& InFile,
	shared_ptr<CModel> model, _fmatrix preTransformMatrix)
{
	Cvt_MeshInfo meshInfo;
	InFile.read(reinterpret_cast<_char*>(&meshInfo), sizeof(Cvt_MeshInfo));

	m_materialIndex = meshInfo.materialIndex;
	
	m_numVertexBuffers = 1; 
	m_numVertices = meshInfo.numVertices;
	//m_iVertexStride = sizeof(VTXMESH);
	m_numIndices = meshInfo.numIndices;
	m_indexStride = 4; // 크게 4바이트로 잡음
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_numBones = meshInfo.numBones;
	memcpy(&m_name, &meshInfo.name, MAX_PATH);




		m_boneIndices.reserve(m_numBones);
		m_offsetMatrices.reserve(m_numBones);

		for (size_t i = 0; i < m_numBones; ++i)
		{
			Cvt_BoneAdd boneAdd = {};
			InFile.read(reinterpret_cast<_char*>(&boneAdd), sizeof(Cvt_BoneAdd));

			// 데이터 벡터에 보관 (나중에 Bind_BoneMatrices에서 사용)
			m_boneIndices.push_back((_uint)boneAdd.boneIndex);
			_float4x4 offset = {  };
			memcpy(&offset, &boneAdd.offsetMatrix, sizeof(_float4x4));
			m_offsetMatrices.push_back(offset);
		}
	


	//vertex Buffer
	HRESULT hr = EModelType::NONANIM == type ? Ready_VertexBuffer_For_NonAnim(InFile, preTransformMatrix)
		: Ready_VertexBuffer_For_Anim(InFile, model);

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


	_uint* indices = new _uint[m_numIndices];
	InFile.read(reinterpret_cast<_char*>(indices), sizeof(_uint) * m_numIndices);

	/*_uint	iNumIndices = { 0 };


	for (_uint i = 0; i < meshInfo.iNumIndices; i++)
	{
		
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}*/

	
	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = indices;

	if (FAILED(m_device->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_IB)))
		return E_FAIL;



	Safe_Delete_Array(indices);


	return S_OK;
}

HRESULT CMesh::Initialize(void* arg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(shared_ptr<CShader> shader, const _char* constantName,
	const vector<shared_ptr<CBone>>& bones)
{
	//ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * 512);
	for (int i = 0; i < 512; i++)
	{
		XMStoreFloat4x4(&m_boneMatrices[i], XMMatrixIdentity());
	}
	for(size_t i =0; i<m_numBones; i++)
	{
		XMStoreFloat4x4(&m_boneMatrices[i], XMLoadFloat4x4(&m_offsetMatrices[i] )*
			XMLoadFloat4x4(bones[m_boneIndices[i]]->Get_CombinedTransformationMatrixPtr()));
		//XMStoreFloat4x4(&m_BoneMatrices[i],
		//	XMLoadFloat4x4(bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr()) *
		//	XMLoadFloat4x4(&m_OffsetMatrices[i]));
	}
	
		return shader->Bind_Matrices(constantName, m_boneMatrices, m_numBones);
}


HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(ifstream& InFile, _fmatrix preTransformMatrix)
{
	// Vertex Buffer

	m_vertexStride = sizeof(VTXMESH);
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;

	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음


	// read
	Cvt_VTXMESH* tmpVertices = new Cvt_VTXMESH[m_numVertices];
	InFile.read(reinterpret_cast<_char*>(tmpVertices), sizeof(Cvt_VTXMESH) * m_numVertices);


	VTXMESH* vertices = new VTXMESH[m_numVertices];

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&vertices[i].position, &tmpVertices[i].pos, sizeof(_float3));
		XMStoreFloat3(&vertices[i].position,
			XMVector3TransformCoord(XMLoadFloat3(&vertices[i].position), preTransformMatrix));

		memcpy(&vertices[i].normal, &tmpVertices[i].normal, sizeof(_float3));
		XMStoreFloat3(&vertices[i].normal,
			XMVector3TransformCoord(XMLoadFloat3(&vertices[i].normal), preTransformMatrix));

		memcpy(&vertices[i].tangent, &tmpVertices[i].tangent, sizeof(_float3));
		XMStoreFloat3(&vertices[i].tangent,
			XMVector3TransformCoord(XMLoadFloat3(&vertices[i].tangent), preTransformMatrix));

		vertices[i].texcoord.x = tmpVertices[i].uv[0][0];
		vertices[i].texcoord.y = tmpVertices[i].uv[0][1];

		
		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}
	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;

	Safe_Delete_Array(vertices);
	Safe_Delete_Array(tmpVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(ifstream& InFile, shared_ptr<CModel> model)
{
	m_vertexStride = sizeof(VTXANIMMESH);
	
	// Vertex Buffer
	
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;
	
	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음
	



	// read
	Cvt_VTXMESH* tmpVertices = new Cvt_VTXMESH[m_numVertices];
	InFile.read(reinterpret_cast<_char*>(tmpVertices), sizeof(Cvt_VTXMESH) * m_numVertices);


	VTXANIMMESH* vertices = new VTXANIMMESH[m_numVertices];

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&vertices[i].position, &tmpVertices[i].pos, sizeof(_float3));
		//XMStoreFloat3(&pVertices[i].vPosition,
		//	XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&vertices[i].normal, &tmpVertices[i].normal, sizeof(_float3));
		//XMStoreFloat3(&pVertices[i].vNormal,
		//	XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&vertices[i].tangent, &tmpVertices[i].tangent, sizeof(_float3));
		//XMStoreFloat3(&pVertices[i].vTangent,
		//	XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

		vertices[i].texcoord.x = tmpVertices[i].uv[0][0];
		vertices[i].texcoord.y = tmpVertices[i].uv[0][1];

		memcpy(&vertices[i].blendIndex, tmpVertices[i].indices, sizeof(_uint) * 4);
		memcpy(&vertices[i].blendWeight, tmpVertices[i].weights, sizeof(_float) * 4);


	}

	if (0 == m_numBones)
	{

		m_boneIndices.push_back(model->Get_BoneIndex(m_name));

		m_numBones = 1;

		_float4x4		offsetMatrix = {};
		XMStoreFloat4x4(&offsetMatrix, XMMatrixIdentity());

		m_offsetMatrices.push_back(offsetMatrix);
		for (size_t i = 0; i < m_numVertices; ++i) {
			vertices[i].blendIndex = { 0, 0, 0, 0 }; // 셰이더가 딴데 안 가게 고정
			vertices[i].blendWeight = _float4(0.f, 0.f, 0.f, 0.f); // x,y,z가 0이면 fWeightW가 1이 됨
		}
		}


	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;


	//for (size_t i = 0; i < m_iNumBones; i++)
	//{
	//	// 이 메쉬가 사용하는 뼈의 이름을 읽거나 인덱스를 읽어서 pModel에서 찾아야 합니다.
	//	// (만약 컨버터에서 본 인덱스를 저장했다면 인덱스를 읽으세요)

	//	// 예시: 뼈의 인덱스와 OffsetMatrix를 읽는 경우
	//	

	//	m_BoneIndices.push_back(tmpVertices->vIndices);

	//	_float4x4 OffsetMatrix;
	//	InFile.read((char*)&OffsetMatrix, sizeof(_float4x4));
	//	m_OffsetMatrices.push_back(OffsetMatrix);
	//}
	
	Safe_Delete_Array(vertices);
	Safe_Delete_Array(tmpVertices);
	return S_OK;

}

shared_ptr<CMesh> CMesh::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, EModelType type, ifstream& InFile , shared_ptr<CModel> model, _fmatrix preTransformMatrix)
{
    shared_ptr<CMesh> instance(new CMesh(device, context), [](CMesh* p) {p->Free(); delete p;});

    if (FAILED(instance->Initialize_Prototype(type, InFile, model, preTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CMesh");
    }
    return instance;

}

shared_ptr<CComponent> CMesh::Clone(void* arg)
{

    shared_ptr<CComponent> instance(new CMesh(*this), [](CMesh* p) {p->Free(); delete p;});

    if (FAILED(instance->Initialize(arg)))
    {
        MSG_BOX("Failed to Cloned : CMesh");
    }
    return instance;
}


void CMesh::Free()
{
    __super::Free();
}
