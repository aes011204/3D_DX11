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
	m_Pivot = Prototype.m_Pivot;
	m_Range = Prototype.m_Range;
	m_SizeRange = Prototype.m_SizeRange;
	m_LifeTimeRange = Prototype.m_LifeTimeRange;
	m_SpeedRange = Prototype.m_SpeedRange;
	m_Angle = Prototype.m_Angle;

	m_pInstanceVertices = make_unique<VTXPARTICLE_INSTANCE[]>(m_iNumInstances);
	m_pSpeeds = make_unique<_float[]>(m_iNumInstances);

	memcpy(m_pInstanceVertices.get(), Prototype.m_pInstanceVertices.get(),
		sizeof(VTXPARTICLE_INSTANCE) * m_iNumInstances);

	memcpy(m_pSpeeds.get(), Prototype.m_pSpeeds.get(),
		sizeof(_float) * m_iNumInstances);
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc)
{
	auto		pDesc = static_cast<const PARTICLE_POINT_DESC*>(pParticleDesc);

	m_isLoop = pDesc->isLoop;
	m_Pivot = pDesc->vPivot;
	m_Range = pDesc->vRange;
	m_SpeedRange = pDesc->vSpeed;
	m_LifeTimeRange = pDesc->vLifeTime;
	m_SizeRange = pDesc->vSizeRange;
	m_Angle = pDesc->Angle;


	/* For.Vertices */
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);

	/* For.Indices */
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	/* For.Instances*/
	m_iNumInstances = pDesc->iNumInstances;
	m_iInstanceStride = sizeof(VTXPARTICLE_INSTANCE);


	// Vertex Buffer

	D3D11_BUFFER_DESC VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VertexBufferDesc.CPUAccessFlags = 0; // 동적 버퍼할떄 의미 있음
	VertexBufferDesc.MiscFlags = 0; // 동적 버퍼할떄 의미 있음

	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);



	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;


	//Index Buffer

	/* Instance Buffer */



	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;


	m_pInstanceVertices = std::make_unique<VTXPARTICLE_INSTANCE[]>(m_iNumInstances);
	ZeroMemory(m_pInstanceVertices.get(), sizeof(VTXPARTICLE_INSTANCE) * m_iNumInstances);
	m_pSpeeds = std::make_unique<_float[]>(m_iNumInstances);

	auto pGameInstance = m_pGameInstance.lock();
	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		_float		fScale = pGameInstance->Random(pDesc->vScale.x, pDesc->vScale.y);
		m_pSpeeds[i] = pGameInstance->Random(pDesc->vSpeed.x, pDesc->vSpeed.y);

		m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = _float4(
			pGameInstance->Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f),
			pGameInstance->Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f),
			pGameInstance->Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f),
			1.f
		);
		m_pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance.lock()->Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y), 0.f);

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
	InstanceInitialData.pSysMem = m_pInstanceVertices.get();

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Particle_Point::Drop(_float fTimeDelta)
{

	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	auto pVertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(MappedSubResource.pData);


	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		pVertexInstance[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;

		pVertexInstance[i].vLifeTime.y += fTimeDelta;
		if (pVertexInstance[i].vLifeTime.y >= pVertexInstance[i].vLifeTime.x &&
			true == m_isLoop)
		{
			pVertexInstance[i].vLifeTime.y = 0.f;
			pVertexInstance[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Particle_Point::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	auto		pVertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertexInstance[i].vTranslation) - XMLoadFloat3(&m_Pivot), 0.f);

		XMStoreFloat4(&pVertexInstance[i].vTranslation,
			XMLoadFloat4(&pVertexInstance[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

		pVertexInstance[i].vLifeTime.y += fTimeDelta;
		if (pVertexInstance[i].vLifeTime.y >= pVertexInstance[i].vLifeTime.x &&
			true == m_isLoop)
		{
			pVertexInstance[i].vLifeTime.y = 0.f;
			pVertexInstance[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Particle_Point::Spawn(_float fTimeDelta)
{
	auto pGameInstance = m_pGameInstance.lock();
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	auto		pVertexInstance = static_cast<VTXPARTICLE_INSTANCE*>(MappedSubResource.pData);

	_matrix matRot = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Angle.x),
		XMConvertToRadians(m_Angle.y),
		XMConvertToRadians(m_Angle.z)
	);
	for (size_t i = 0; i < m_iNumInstances; i++)
	{

		pVertexInstance[i].vLifeTime.y += fTimeDelta;
		// y 가 현제 시간 / x가 전체 시간

		if (pVertexInstance[i].vLifeTime.y >= pVertexInstance[i].vLifeTime.x &&
			true == m_isLoop)
		{
			pVertexInstance[i].vLifeTime.y = 0.f;
			pVertexInstance[i].vTranslation = _float4(
				pGameInstance->Random(m_Pivot.x - m_Range.x * 0.5f, m_Pivot.x + m_Range.x * 0.5f),
				pGameInstance->Random(m_Pivot.y - m_Range.y * 0.5f, m_Pivot.y + m_Range.y * 0.5f),
				pGameInstance->Random(m_Pivot.z - m_Range.z * 0.5f, m_Pivot.z + m_Range.z * 0.5f),
				1.f
			);

			pVertexInstance[i].vLifeTime.x = pGameInstance->Random(m_LifeTimeRange.x, m_LifeTimeRange.y);

			//m_pSpeeds[i] = pGameInstance->Random(m_SpeedRange.x, m_SpeedRange.y);



		}


		float lifeRatio = pVertexInstance[i].vLifeTime.y / pVertexInstance[i].vLifeTime.x;
		lifeRatio = min(lifeRatio, 1.f);


		float fScale = m_SizeRange.x + (m_SizeRange.y - m_SizeRange.x) * lifeRatio;

		//float fAlpha = 1.f - lifeRatio;

		_vector vRight = XMVector3TransformNormal(XMVectorSet(fScale, 0.f, 0.f, 0.f), matRot);
		_vector vUp = XMVector3TransformNormal(XMVectorSet(0.f, fScale, 0.f, 0.f), matRot);
		_vector vLook = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, fScale, 0.f), matRot);

		// 3. 인스턴스 버퍼에 그대로 대입 (GPU 전달)
		XMStoreFloat4(&pVertexInstance[i].vRight, vRight);
		XMStoreFloat4(&pVertexInstance[i].vUp, vUp);
		XMStoreFloat4(&pVertexInstance[i].vLook, vLook);

	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}


HRESULT CVIBuffer_Particle_Point::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB.Get(),
		m_pVBInstance.Get(),
	};

	_uint			iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint			iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstances, 0, 0);

	return S_OK;
}

shared_ptr<CVIBuffer_Particle_Point> CVIBuffer_Particle_Point::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc)
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
