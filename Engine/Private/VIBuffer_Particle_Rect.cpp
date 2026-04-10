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


	m_pInstanceVertices = std::make_unique<VTXPARTICLE_INSTANCE[]>(m_iNumInstances);
	m_pSpeeds = std::make_unique<_float[]>(m_iNumInstances);

	memcpy(m_pInstanceVertices.get(), Prototype.m_pInstanceVertices.get(),
		sizeof(VTXPARTICLE_INSTANCE) * m_iNumInstances);

	memcpy(m_pSpeeds.get(), Prototype.m_pSpeeds.get(),
		sizeof(_float) * m_iNumInstances);
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc)
{
	auto		pDesc = static_cast<const PARTICLE_RECT_DESC*>(pParticleDesc);

	m_isLoop = pDesc->isLoop;


	/* For.Vertices */
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXTEX);
	/* For.Indices */
	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/* For.Instances*/
	m_iIndexCountPerInstance = 6;
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
	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);


	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;


	//Index Buffer

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_iIndexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;


	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
		return E_FAIL;

	/* Instance Buffer */


	D3D11_BUFFER_DESC InstanceBufferDesc{};
	InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceBufferDesc.MiscFlags = 0;


	m_pInstanceVertices = std::make_unique<VTXPARTICLE_INSTANCE[]>(m_iNumInstances);
	ZeroMemory(m_pInstanceVertices.get(), sizeof(VTXPARTICLE_INSTANCE) * m_iNumInstances);
	m_pSpeeds = std::make_unique<_float[]>(m_iNumInstances);

	 auto pGameInstance = m_pGameInstance.lock();
	for(size_t i=0; i<m_iNumInstances;i++)
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


	D3D11_SUBRESOURCE_DATA			InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices.get();

	if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
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

void CVIBuffer_Particle_Rect::Spread(_float fTimeDelta)
{
}

shared_ptr<CVIBuffer_Particle_Rect> CVIBuffer_Particle_Rect::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc)
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
