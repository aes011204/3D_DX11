#include "VIBuffer_Particle_Rect.h"
#include "GameInstance.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CVIBuffer_Instance{ device, context }
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

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(const CVIBuffer_Instance::InstanceDesc* particleDesc)
{
	auto		desc = static_cast<const ParticleRectDesc*>(particleDesc);

	m_isLoop = desc->isLoop;


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
	m_numInstances = desc->numInstances;
	m_instanceStride = sizeof(VTXPARTICLE_INSTANCE);

	// Vertex Buffer

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_vertexStride;

	vertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	vertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXTEX* vertices = new VTXTEX[m_numVertices];
	vertices[0].position = _float3(-0.5f, 0.5f, 0.f);
	vertices[0].texcoord = _float2(0.f, 0.f);

	vertices[1].position = _float3(0.5f, 0.5f, 0.f);
	vertices[1].texcoord = _float2(1.f, 0.f);

	vertices[2].position = _float3(0.5f, -0.5f, 0.f);
	vertices[2].texcoord = _float2(1.f, 1.f);

	vertices[3].position = _float3(-0.5f, -0.5f, 0.f);
	vertices[3].texcoord = _float2(0.f, 1.f);


	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;


	//Index Buffer

	D3D11_BUFFER_DESC	indexBufferDesc{};
	indexBufferDesc.ByteWidth = m_indexStride * m_numIndices;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = m_indexStride;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	_ushort* indices = new _ushort[m_numIndices];

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = indices;


	if (FAILED(m_device->CreateBuffer(&indexBufferDesc, &indexInitialData, &m_IB)))
		return E_FAIL;

	/* Instance Buffer */


	D3D11_BUFFER_DESC instanceBufferDesc{};
	instanceBufferDesc.ByteWidth = m_instanceStride * m_numInstances;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.StructureByteStride = m_instanceStride;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;


	m_instanceVertices = std::make_unique<VTXPARTICLE_INSTANCE[]>(m_numInstances);
	ZeroMemory(m_instanceVertices.get(), sizeof(VTXPARTICLE_INSTANCE) * m_numInstances);
	m_speeds = std::make_unique<_float[]>(m_numInstances);

	 auto gameInstance = m_gameInstance.lock();
	for(size_t i=0; i<m_numInstances;i++)
	{
		


		m_speeds[i] = gameInstance->Random(desc->speed.x, desc->speed.y);
		_float scaleX = .02f;
		_float scaleY = gameInstance->Random(0.4f, 0.6f);
	//	_float fScaleZ = pGameInstance->Random(pDesc->vScaleZ.x, pDesc->vScaleZ.y); // 필요하면

		_float life = m_gameInstance.lock()->Random(desc->lifeTime.x, desc->lifeTime.y);
		_float start = m_gameInstance.lock()->Random(0.f, life);

		m_instanceVertices[i].lifeTime = _float2(life, start);


		m_instanceVertices[i].right = _float4(scaleX, 0.f, 0.f, 0.f);
		m_instanceVertices[i].up = _float4(0.f, scaleY, 0.f, 0.f);
		m_instanceVertices[i].look = _float4(0.f, 0.f, 1.f, 0.f);
		m_instanceVertices[i].translation = _float4(
			gameInstance->Random(desc->center.x - desc->range.x * 0.5f, desc->center.x + desc->range.x * 0.5f),
			gameInstance->Random(desc->center.y - desc->range.y * 0.5f, desc->center.y + desc->range.y * 0.5f),
			gameInstance->Random(desc->center.z - desc->range.z * 0.5f, desc->center.z + desc->range.z * 0.5f),
			1.f
		);
	//	m_pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance.lock()->Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y), 0.f);
	
	}


	D3D11_SUBRESOURCE_DATA			instanceInitialData{};
	instanceInitialData.pSysMem = m_instanceVertices.get();

	if (FAILED(m_device->CreateBuffer(&instanceBufferDesc, &instanceInitialData, &m_VBInstance)))
		return E_FAIL;

	Safe_Delete_Array(vertices);
	Safe_Delete_Array(indices);


	return S_OK;

}

HRESULT CVIBuffer_Particle_Rect::Initialize(void* arg)
{
	return S_OK;
}

void CVIBuffer_Particle_Rect::Drop(_float timeDelta)
{

	D3D11_MAPPED_SUBRESOURCE mappedSubResource{};

	m_context->Map(m_VBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedSubResource);

	auto vertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(mappedSubResource.pData);

	for (size_t i = 0; i < m_numInstances; i++)
	{

		vertexInstance[i].translation.y -= m_speeds[i] * timeDelta;

		vertexInstance[i].lifeTime.y += timeDelta;
		if (vertexInstance[i].lifeTime.y >= vertexInstance[i].lifeTime.x &&
			true == m_isLoop)
		{
			vertexInstance[i].lifeTime.y = 0.f;
			vertexInstance[i].translation = m_instanceVertices[i].translation;
		}
	}

	m_context->Unmap(m_VBInstance.Get(), 0);
}

void CVIBuffer_Particle_Rect::Spread(_float timeDelta)
{
}

shared_ptr<CVIBuffer_Particle_Rect> CVIBuffer_Particle_Rect::Create(ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> context, const CVIBuffer_Instance::InstanceDesc* particleDesc)
{
	shared_ptr<CVIBuffer_Particle_Rect> instance(new CVIBuffer_Particle_Rect(device, context), [](CVIBuffer_Particle_Rect* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype(particleDesc)))
	{
		MSG_BOX("Failed to Created : CUIPanel");
	
	}
	return instance;
}

shared_ptr<CComponent> CVIBuffer_Particle_Rect::Clone(void* arg)
{
	shared_ptr<CComponent> instance(new CVIBuffer_Particle_Rect(*this), [](CVIBuffer_Particle_Rect* p) {p->Free(); delete(p); });
	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("failed clone: CUITransform");
	}
	return instance;
}

void CVIBuffer_Particle_Rect::Free()
{
	CVIBuffer_Instance::Free();


}
