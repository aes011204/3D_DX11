#include "Mesh.h"
#include "Model.h"
#include "Shader.h"

CMesh::CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& Prototype)
    : CVIBuffer(Prototype)
{
}

HRESULT CMesh::Initialize_Prototype(EModelType eType,
	 ifstream& InFile,
	shared_ptr<CModel> pModel, _fmatrix PreTransformMatrix)
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
	HRESULT hr = EModelType::NONANIM == eType ? Ready_VertexBuffer_For_NonAnim(InFile, PreTransformMatrix)
		: Ready_VertexBuffer_For_Anim(InFile, pModel);

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


	_uint* pIndices = new _uint[m_numIndices];
	InFile.read(reinterpret_cast<_char*>(pIndices), sizeof(_uint) * m_numIndices);

	/*_uint	iNumIndices = { 0 };


	for (_uint i = 0; i < meshInfo.iNumIndices; i++)
	{
		
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}*/

	
	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_device->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_IB)))
		return E_FAIL;



	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(shared_ptr<CShader> shader, const _char* pConstantName,
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
	
		return shader->Bind_Matrices(pConstantName, m_boneMatrices, m_numBones);
}


HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(ifstream& InFile, _fmatrix PreTransformMatrix)
{
	// Vertex Buffer

	m_vertexStride = sizeof(VTXMESH);
	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음


	// read
	Cvt_VTXMESH* tmpVertices = new Cvt_VTXMESH[m_numVertices];
	InFile.read(reinterpret_cast<_char*>(tmpVertices), sizeof(Cvt_VTXMESH) * m_numVertices);


	VTXMESH* pVertices = new VTXMESH[m_numVertices];

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&pVertices[i].position, &tmpVertices[i].pos, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].position,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].position), PreTransformMatrix));

		memcpy(&pVertices[i].normal, &tmpVertices[i].normal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].normal,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].normal), PreTransformMatrix));

		memcpy(&pVertices[i].tangent, &tmpVertices[i].tangent, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].tangent,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].tangent), PreTransformMatrix));

		pVertices[i].texcoord.x = tmpVertices[i].uv[0][0];
		pVertices[i].texcoord.y = tmpVertices[i].uv[0][1];

		
		// 덱스쿠드는 여러개일수도 있으니 이차배열
		// 하나의 세트인데 바디 에 디퓨즈 , 노말의 모양이 다르다면 , 텍스쿠드를더 선언해야함
		// 큐브 라던지 그건 xyz 다있음 
	}
	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(tmpVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(ifstream& InFile, shared_ptr<CModel> pModel)
{
	m_vertexStride = sizeof(VTXANIMMESH);
	
	// Vertex Buffer
	
	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;
	
	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음
	



	// read
	Cvt_VTXMESH* tmpVertices = new Cvt_VTXMESH[m_numVertices];
	InFile.read(reinterpret_cast<_char*>(tmpVertices), sizeof(Cvt_VTXMESH) * m_numVertices);


	VTXANIMMESH* pVertices = new VTXANIMMESH[m_numVertices];

	for (size_t i = 0; i < m_numVertices; i++)
	{
		memcpy(&pVertices[i].position, &tmpVertices[i].pos, sizeof(_float3));
		//XMStoreFloat3(&pVertices[i].vPosition,
		//	XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].normal, &tmpVertices[i].normal, sizeof(_float3));
		//XMStoreFloat3(&pVertices[i].vNormal,
		//	XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].tangent, &tmpVertices[i].tangent, sizeof(_float3));
		//XMStoreFloat3(&pVertices[i].vTangent,
		//	XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

		pVertices[i].texcoord.x = tmpVertices[i].uv[0][0];
		pVertices[i].texcoord.y = tmpVertices[i].uv[0][1];

		memcpy(&pVertices[i].blendIndex, tmpVertices[i].indices, sizeof(_uint) * 4);
		memcpy(&pVertices[i].blendWeight, tmpVertices[i].weights, sizeof(_float) * 4);


	}

	if (0 == m_numBones)
	{

		m_boneIndices.push_back(pModel->Get_BoneIndex(m_name));

		m_numBones = 1;

		_float4x4		OffsetMatrix = {};
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_offsetMatrices.push_back(OffsetMatrix);
		for (size_t i = 0; i < m_numVertices; ++i) {
			pVertices[i].blendIndex = { 0, 0, 0, 0 }; // 셰이더가 딴데 안 가게 고정
			pVertices[i].blendWeight = _float4(0.f, 0.f, 0.f, 0.f); // x,y,z가 0이면 fWeightW가 1이 됨
		}
		}


	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
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
	
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(tmpVertices);
	return S_OK;

}

shared_ptr<CMesh> CMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, EModelType eType, ifstream& InFile , shared_ptr<CModel> pModel, _fmatrix PreTransformMatrix)
{
    shared_ptr<CMesh> pInstance(new CMesh(pDevice, pContext), [](CMesh* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize_Prototype(eType, InFile, pModel, PreTransformMatrix)))
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
