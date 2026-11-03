#include "VIBuffer_Particle_Rect.h"
#include "GameInstance.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype)
	: CVIBuffer_Instance{ Prototype }
	//, m_pSpeeds{ Prototype.m_pSpeeds }
	//, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_isLoop{ Prototype.m_isLoop }
	
{
	//m_isLoop = Prototype.m_isLoop;


	m_instanceVertices = std::make_unique<VTXPARTICLE_INSTANCE[]>(m_numInstances);
	m_speeds = std::make_unique<_float[]>(m_numInstances);

	memcpy(m_instanceVertices.get(), Prototype.m_instanceVertices.get(),
		sizeof(VTXPARTICLE_INSTANCE) * m_numInstances);

	memcpy(m_speeds.get(), Prototype.m_speeds.get(),
		sizeof(_float) * m_numInstances);
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(const CVIBuffer_Instance::InstanceDesc* pParticleDesc)
{
	auto		pDesc = static_cast<const ParticleRectDesc*>(pParticleDesc);

	m_isLoop = pDesc->isLoop;


	/* For.Vertices */
	m_numVertexBuffers = 2;
	m_numVertices = 4;
	m_vertexStride = sizeof(VTXTEX);
	/* For.Indices */
	m_numIndices = 6;
	m_indexStride = 2;
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/* For.Instances*/
	m_indexCountPerInstance = 6;
	m_numInstances = pDesc->numInstances;
	m_instanceStride = sizeof(VTXPARTICLE_INSTANCE);

	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_vertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXTEX* pVertices = new VTXTEX[m_numVertices];
	pVertices[0].position = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].texcoord = _float2(0.f, 0.f);

	pVertices[1].position = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].texcoord = _float2(1.f, 0.f);

	pVertices[2].position = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].texcoord = _float2(1.f, 1.f);

	pVertices[3].position = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].texcoord = _float2(0.f, 1.f);


	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
		return E_FAIL;


	//Index Buffer

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_indexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_numIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;


	if (FAILED(m_device->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_IB)))
		return E_FAIL;

	/* Instance Buffer */


	D3D11_BUFFER_DESC InstanceBufferDesc{};
	InstanceBufferDesc.ByteWidth = m_instanceStride * m_numInstances;
	InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.StructureByteStride = m_instanceStride;
	InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceBufferDesc.MiscFlags = 0;


	m_instanceVertices = std::make_unique<VTXPARTICLE_INSTANCE[]>(m_numInstances);
	ZeroMemory(m_instanceVertices.get(), sizeof(VTXPARTICLE_INSTANCE) * m_numInstances);
	m_speeds = std::make_unique<_float[]>(m_numInstances);

	 auto pGameInstance = m_gameInstance.lock();
	for(size_t i=0; i<m_numInstances;i++)
	{
		


		m_speeds[i] = pGameInstance->Random(pDesc->speed.x, pDesc->speed.y);
		_float fScaleX = .02f;
		_float fScaleY = pGameInstance->Random(0.4f, 0.6f);
	//	_float fScaleZ = pGameInstance->Random(pDesc->vScaleZ.x, pDesc->vScaleZ.y); // 필요하면

		_float life = m_gameInstance.lock()->Random(pDesc->lifeTime.x, pDesc->lifeTime.y);
		_float start = m_gameInstance.lock()->Random(0.f, life);

		m_instanceVertices[i].lifeTime = _float2(life, start);


		m_instanceVertices[i].right = _float4(fScaleX, 0.f, 0.f, 0.f);
		m_instanceVertices[i].up = _float4(0.f, fScaleY, 0.f, 0.f);
		m_instanceVertices[i].look = _float4(0.f, 0.f, 1.f, 0.f);
		m_instanceVertices[i].translation = _float4(
			pGameInstance->Random(pDesc->center.x - pDesc->range.x * 0.5f, pDesc->center.x + pDesc->range.x * 0.5f),
			pGameInstance->Random(pDesc->center.y - pDesc->range.y * 0.5f, pDesc->center.y + pDesc->range.y * 0.5f),
			pGameInstance->Random(pDesc->center.z - pDesc->range.z * 0.5f, pDesc->center.z + pDesc->range.z * 0.5f),
			1.f
		);
	//	m_pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance.lock()->Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y), 0.f);
	
	}


	D3D11_SUBRESOURCE_DATA			InstanceInitialData{};
	InstanceInitialData.pSysMem = m_instanceVertices.get();

	if (FAILED(m_device->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_VBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


	return S_OK;

}

HRESULT CVIBuffer_Particle_Rect::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Particle_Rect::Drop(_float fTimeDelta)
{

	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_context->Map(m_VBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	auto pVertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_numInstances; i++)
	{

		pVertexInstance[i].translation.y -= m_speeds[i] * fTimeDelta;

		pVertexInstance[i].lifeTime.y += fTimeDelta;
		if (pVertexInstance[i].lifeTime.y >= pVertexInstance[i].lifeTime.x &&
			true == m_isLoop)
		{
			pVertexInstance[i].lifeTime.y = 0.f;
			pVertexInstance[i].translation = m_instanceVertices[i].translation;
		}
	}

	m_context->Unmap(m_VBInstance.Get(), 0);
}

void CVIBuffer_Particle_Rect::Spread(_float fTimeDelta)
{
}

shared_ptr<CVIBuffer_Particle_Rect> CVIBuffer_Particle_Rect::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::InstanceDesc* pParticleDesc)
{
	shared_ptr<CVIBuffer_Particle_Rect> pInstance(new CVIBuffer_Particle_Rect(pDevice, pContext), [](CVIBuffer_Particle_Rect* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype(pParticleDesc)))
	{
		MSG_BOX("Failed to Created : CUIPanel");
	
	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Particle_Rect::Clone(void* pArg)
{
	shared_ptr<CComponent> pInstance(new CVIBuffer_Particle_Rect(*this), [](CVIBuffer_Particle_Rect* p) {p->Free(); delete(p); });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("failed clone: CUITransform");
	}
	return pInstance;
}

void CVIBuffer_Particle_Rect::Free()
{
	CVIBuffer_Instance::Free();


}
