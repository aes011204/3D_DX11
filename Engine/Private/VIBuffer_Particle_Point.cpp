#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
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

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const CVIBuffer_Instance::InstanceDesc* pParticleDesc)
{
	auto		pDesc = static_cast<const ParticlePointDesc*>(pParticleDesc);

	m_isLoop = pDesc->isLoop;
	m_pivot = pDesc->pivot;
	m_range = pDesc->range;
	m_speedRange = pDesc->speed;
	m_lifeTimeRange = pDesc->lifeTime;
	m_sizeRange = pDesc->sizeRange;
	m_angle = pDesc->angle;


	/* For.Vertices */
	m_numVertexBuffers = 2;
	m_numVertices = 1;
	m_vertexStride = sizeof(VTXPOS);

	/* For.Indices */
	m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	/* For.Instances*/
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
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_numVertices);



	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_device->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_VB)))
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

	auto pGameInstance = m_gameInstance.lock();
	for (size_t i = 0; i < m_numInstances; i++)
	{
		_float		fScale = pGameInstance->Random(pDesc->scale.x, pDesc->scale.y);
		m_speeds[i] = pGameInstance->Random(pDesc->speed.x, pDesc->speed.y);

		m_instanceVertices[i].right = _float4(fScale, 0.f, 0.f, 0.f);
		m_instanceVertices[i].up = _float4(0.f, fScale, 0.f, 0.f);
		m_instanceVertices[i].look = _float4(0.f, 0.f, fScale, 0.f);
		m_instanceVertices[i].translation = _float4(
			pGameInstance->Random(pDesc->center.x - pDesc->range.x * 0.5f, pDesc->center.x + pDesc->range.x * 0.5f),
			pGameInstance->Random(pDesc->center.y - pDesc->range.y * 0.5f, pDesc->center.y + pDesc->range.y * 0.5f),
			pGameInstance->Random(pDesc->center.z - pDesc->range.z * 0.5f, pDesc->center.z + pDesc->range.z * 0.5f),
			1.f
		);
		m_instanceVertices[i].lifeTime = _float2(m_gameInstance.lock()->Random(pDesc->lifeTime.x, pDesc->lifeTime.y), 0.f);

	}


	//D3D11_SUBRESOURCE_DATA			InstanceInitialData{};
	//InstanceInitialData.pSysMem = m_pInstanceVertices.get();

	//if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
	//	return E_FAIL;

	Safe_Delete_Array(pVertices);



	return S_OK;

}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
	D3D11_SUBRESOURCE_DATA			InstanceInitialData{};
	InstanceInitialData.pSysMem = m_instanceVertices.get();

	if (FAILED(m_device->CreateBuffer(&m_instanceBufferDesc, &InstanceInitialData, &m_VBInstance)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Particle_Point::Drop(_float fTimeDelta)
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

void CVIBuffer_Particle_Point::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_context->Map(m_VBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	auto		pVertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_numInstances; i++)
	{
		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertexInstance[i].translation) - XMLoadFloat3(&m_pivot), 0.f);

		XMStoreFloat4(&pVertexInstance[i].translation,
			XMLoadFloat4(&pVertexInstance[i].translation) + XMVector3Normalize(vDir) * m_speeds[i] * fTimeDelta);

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

void CVIBuffer_Particle_Point::Spawn(_float fTimeDelta)
{
	auto pGameInstance = m_gameInstance.lock();
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_context->Map(m_VBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	auto		pVertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(MappedSubResource.pData);

	_matrix matRot = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_angle.x),
		XMConvertToRadians(m_angle.y),
		XMConvertToRadians(m_angle.z)
	);

	bool hasAliveParticle = false;


	for (size_t i = 0; i < m_numInstances; i++)
	{

		pVertexInstance[i].lifeTime.y += fTimeDelta;
		// y 가 현제 시간 / x가 전체 시간

		if (pVertexInstance[i].lifeTime.y >= pVertexInstance[i].lifeTime.x &&
			true == m_isLoop)
		{
			pVertexInstance[i].lifeTime.y = 0.f;
			pVertexInstance[i].translation = _float4(
				pGameInstance->Random(m_pivot.x - m_range.x * 0.5f, m_pivot.x + m_range.x * 0.5f),
				pGameInstance->Random(m_pivot.y - m_range.y * 0.5f, m_pivot.y + m_range.y * 0.5f),
				pGameInstance->Random(m_pivot.z - m_range.z * 0.5f, m_pivot.z + m_range.z * 0.5f),
				1.f
			);

			pVertexInstance[i].lifeTime.x = pGameInstance->Random(m_lifeTimeRange.x, m_lifeTimeRange.y);

			//m_pSpeeds[i] = pGameInstance->Random(m_SpeedRange.x, m_SpeedRange.y);



		}


		float lifeRatio = pVertexInstance[i].lifeTime.y / pVertexInstance[i].lifeTime.x;
		lifeRatio = min(lifeRatio, 1.f);


		float fScale = m_sizeRange.x + (m_sizeRange.y - m_sizeRange.x) * lifeRatio;

		//float fAlpha = 1.f - lifeRatio;

		_vector vRight = XMVector3TransformNormal(XMVectorSet(fScale, 0.f, 0.f, 0.f), matRot);
		_vector vUp = XMVector3TransformNormal(XMVectorSet(0.f, fScale, 0.f, 0.f), matRot);
		_vector vLook = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, fScale, 0.f), matRot);

		// 3. 인스턴스 버퍼에 그대로 대입 (GPU 전달)
		XMStoreFloat4(&pVertexInstance[i].right, vRight);
		XMStoreFloat4(&pVertexInstance[i].up, vUp);
		XMStoreFloat4(&pVertexInstance[i].look, vLook);

	if (pVertexInstance[i].lifeTime.y < pVertexInstance[i].lifeTime.x)
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
	ID3D11Buffer* pVertexBuffers[] = {
		m_VB.Get(),
		m_VBInstance.Get(),
	};

	_uint			iVertexStrides[] = {
		m_vertexStride,
		m_instanceStride
	};

	_uint			iOffsets[] = {
		0,
		0
	};

	m_context->IASetVertexBuffers(0, m_numVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_context->IASetPrimitiveTopology(m_primitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Render()
{
	m_context->DrawInstanced(1, m_numInstances, 0, 0);

	return S_OK;
}

shared_ptr<CVIBuffer_Particle_Point> CVIBuffer_Particle_Point::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::InstanceDesc* pParticleDesc)
{
	shared_ptr<CVIBuffer_Particle_Point> pInstance(new CVIBuffer_Particle_Point(pDevice, pContext), [](CVIBuffer_Particle_Point* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype(pParticleDesc)))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Particle_Point::Clone(void* pArg)
{
	shared_ptr<CComponent> pInstance(new CVIBuffer_Particle_Point(*this), [](CVIBuffer_Particle_Point* p) {p->Free(); delete(p); });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("failed clone: CUITransform");
	}
	return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{
	CVIBuffer_Instance::Free();


}
