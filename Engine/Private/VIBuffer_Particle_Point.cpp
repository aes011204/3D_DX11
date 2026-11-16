#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CVIBuffer_Instance{ device, context }
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype)
	: CVIBuffer_Instance{ Prototype }
	//, m_pSpeeds{ Prototype.m_pSpeeds }
	//, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_isLoop{ Prototype.m_isLoop }

{
	//m_isLoop = Prototype.m_isLoop;
	m_pivot = Prototype.m_pivot;
	m_range = Prototype.m_range;
	m_sizeRange = Prototype.m_sizeRange;
	m_lifeTimeRange = Prototype.m_lifeTimeRange;
	m_speedRange = Prototype.m_speedRange;
	m_angle = Prototype.m_angle;

	m_instanceVertices = make_unique<VTXPARTICLE_INSTANCE[]>(m_numInstances);
	m_speeds = make_unique<_float[]>(m_numInstances);

	memcpy(m_instanceVertices.get(), Prototype.m_instanceVertices.get(),
		sizeof(VTXPARTICLE_INSTANCE) * m_numInstances);

	memcpy(m_speeds.get(), Prototype.m_speeds.get(),
		sizeof(_float) * m_numInstances);
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const CVIBuffer_Instance::InstanceDesc* particleDesc)
{
	auto		desc = static_cast<const ParticlePointDesc*>(particleDesc);

	m_isLoop = desc->isLoop;
	m_pivot = desc->pivot;
	m_range = desc->range;
	m_speedRange = desc->speed;
	m_lifeTimeRange = desc->lifeTime;
	m_sizeRange = desc->sizeRange;
	m_angle = desc->angle;


	/* For.Vertices */
	m_numVertexBuffers = 2;
	m_numVertices = 1;
	m_vertexStride = sizeof(VTXPOS);

	/* For.Indices */
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	/* For.Instances*/
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
	ZeroMemory(vertices, sizeof(VTXPOS) * m_numVertices);



	D3D11_SUBRESOURCE_DATA			vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
		return E_FAIL;


	//Index Buffer

	/* Instance Buffer */



	m_instanceBufferDesc.ByteWidth = m_instanceStride * m_numInstances;
	m_instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_instanceBufferDesc.StructureByteStride = m_instanceStride;
	m_instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_instanceBufferDesc.MiscFlags = 0;


	m_instanceVertices = std::make_unique<VTXPARTICLE_INSTANCE[]>(m_numInstances);
	ZeroMemory(m_instanceVertices.get(), sizeof(VTXPARTICLE_INSTANCE) * m_numInstances);
	m_speeds = std::make_unique<_float[]>(m_numInstances);

	auto gameInstance = m_gameInstance.lock();
	for (size_t i = 0; i < m_numInstances; i++)
	{
		_float		scale = gameInstance->Random(desc->scale.x, desc->scale.y);
		m_speeds[i] = gameInstance->Random(desc->speed.x, desc->speed.y);

		m_instanceVertices[i].right = _float4(scale, 0.f, 0.f, 0.f);
		m_instanceVertices[i].up = _float4(0.f, scale, 0.f, 0.f);
		m_instanceVertices[i].look = _float4(0.f, 0.f, scale, 0.f);
		m_instanceVertices[i].translation = _float4(
			gameInstance->Random(desc->center.x - desc->range.x * 0.5f, desc->center.x + desc->range.x * 0.5f),
			gameInstance->Random(desc->center.y - desc->range.y * 0.5f, desc->center.y + desc->range.y * 0.5f),
			gameInstance->Random(desc->center.z - desc->range.z * 0.5f, desc->center.z + desc->range.z * 0.5f),
			1.f
		);
		m_instanceVertices[i].lifeTime = _float2(m_gameInstance.lock()->Random(desc->lifeTime.x, desc->lifeTime.y), 0.f);

	}


	//D3D11_SUBRESOURCE_DATA			InstanceInitialData{};
	//InstanceInitialData.pSysMem = m_pInstanceVertices.get();

	//if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
	//	return E_FAIL;

	Safe_Delete_Array(vertices);



	return S_OK;

}

HRESULT CVIBuffer_Particle_Point::Initialize(void* arg)
{
	D3D11_SUBRESOURCE_DATA			instanceInitialData{};
	instanceInitialData.pSysMem = m_instanceVertices.get();

	if (FAILED(m_device->CreateBuffer(&m_instanceBufferDesc, &instanceInitialData, &m_VBInstance)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Particle_Point::Drop(_float timeDelta)
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

void CVIBuffer_Particle_Point::Spread(_float timeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		mappedSubResource{};

	m_context->Map(m_VBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedSubResource);

	auto		vertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(mappedSubResource.pData);

	for (size_t i = 0; i < m_numInstances; i++)
	{
		_vector			dir = XMVectorSetW(XMLoadFloat4(&vertexInstance[i].translation) - XMLoadFloat3(&m_pivot), 0.f);

		XMStoreFloat4(&vertexInstance[i].translation,
			XMLoadFloat4(&vertexInstance[i].translation) + XMVector3Normalize(dir) * m_speeds[i] * timeDelta);

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

void CVIBuffer_Particle_Point::Spawn(_float timeDelta)
{
	auto gameInstance = m_gameInstance.lock();
	D3D11_MAPPED_SUBRESOURCE		mappedSubResource{};

	m_context->Map(m_VBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedSubResource);

	auto		vertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(mappedSubResource.pData);

	_matrix matRot = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_angle.x),
		XMConvertToRadians(m_angle.y),
		XMConvertToRadians(m_angle.z)
	);

	bool hasAliveParticle = false;


	for (size_t i = 0; i < m_numInstances; i++)
	{

		vertexInstance[i].lifeTime.y += timeDelta;
		// y 가 현제 시간 / x가 전체 시간

		if (vertexInstance[i].lifeTime.y >= vertexInstance[i].lifeTime.x &&
			true == m_isLoop)
		{
			vertexInstance[i].lifeTime.y = 0.f;
			vertexInstance[i].translation = _float4(
				gameInstance->Random(m_pivot.x - m_range.x * 0.5f, m_pivot.x + m_range.x * 0.5f),
				gameInstance->Random(m_pivot.y - m_range.y * 0.5f, m_pivot.y + m_range.y * 0.5f),
				gameInstance->Random(m_pivot.z - m_range.z * 0.5f, m_pivot.z + m_range.z * 0.5f),
				1.f
			);

			vertexInstance[i].lifeTime.x = gameInstance->Random(m_lifeTimeRange.x, m_lifeTimeRange.y);

			//m_pSpeeds[i] = pGameInstance->Random(m_SpeedRange.x, m_SpeedRange.y);



		}


		float lifeRatio = vertexInstance[i].lifeTime.y / vertexInstance[i].lifeTime.x;
		lifeRatio = min(lifeRatio, 1.f);


		float scale = m_sizeRange.x + (m_sizeRange.y - m_sizeRange.x) * lifeRatio;

		//float fAlpha = 1.f - lifeRatio;

		_vector right = XMVector3TransformNormal(XMVectorSet(scale, 0.f, 0.f, 0.f), matRot);
		_vector up = XMVector3TransformNormal(XMVectorSet(0.f, scale, 0.f, 0.f), matRot);
		_vector look = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, scale, 0.f), matRot);

		// 3. 인스턴스 버퍼에 그대로 대입 (GPU 전달)
		XMStoreFloat4(&vertexInstance[i].right, right);
		XMStoreFloat4(&vertexInstance[i].up, up);
		XMStoreFloat4(&vertexInstance[i].look, look);

	if (vertexInstance[i].lifeTime.y < vertexInstance[i].lifeTime.x)
	{
		hasAliveParticle = true;
	}
	}

	if (m_isLoop == false && hasAliveParticle == false)
		m_isFinished = true;
	else
		m_isFinished = false;
	m_context->Unmap(m_VBInstance.Get(), 0);
}


HRESULT CVIBuffer_Particle_Point::Bind_Resources()
{
	ID3D11Buffer* vertexBuffers[] = {
		m_VB.Get(),
		m_VBInstance.Get(),
	};

	_uint			vertexStrides[] = {
		m_vertexStride,
		m_instanceStride
	};

	_uint			offsets[] = {
		0,
		0
	};

	m_context->IASetVertexBuffers(0, m_numVertexBuffers, vertexBuffers, vertexStrides, offsets);
	m_context->IASetPrimitiveTopology(m_primitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Render()
{
	m_context->DrawInstanced(1, m_numInstances, 0, 0);

	return S_OK;
}

shared_ptr<CVIBuffer_Particle_Point> CVIBuffer_Particle_Point::Create(ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> context, const CVIBuffer_Instance::InstanceDesc* particleDesc)
{
	shared_ptr<CVIBuffer_Particle_Point> instance(new CVIBuffer_Particle_Point(device, context), [](CVIBuffer_Particle_Point* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype(particleDesc)))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return instance;
}

shared_ptr<CComponent> CVIBuffer_Particle_Point::Clone(void* arg)
{
	shared_ptr<CComponent> instance(new CVIBuffer_Particle_Point(*this), [](CVIBuffer_Particle_Point* p) {p->Free(); delete(p); });
	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("failed clone: CUITransform");
	}
	return instance;
}

void CVIBuffer_Particle_Point::Free()
{
	CVIBuffer_Instance::Free();


}
