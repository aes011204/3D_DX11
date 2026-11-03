#pragma once
#include "VIBuffer_Instance.h"


NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Point :
	public CVIBuffer_Instance
{

public:
	struct ParticlePointDesc : public CVIBuffer_Instance::InstanceDesc
	{

		_float3	pivot;
		_float2 speed;
		_float2 lifeTime;
		_float2 sizeRange;
		_bool isLoop;
		_float3 angle;

	};

protected:
	CVIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype);
public:
	virtual ~CVIBuffer_Particle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instance::InstanceDesc* pParticleDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;
public:
	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);
	void Spawn(_float fTimeDelta);

	void Stop() { m_isLoop = false; };
	void Start() { m_isLoop = true; };

	void Set_Pivot(const _float3& pivot) { m_pivot = pivot; }
	_bool Is_Finished() { return m_isFinished; };
protected:

protected:
	unique_ptr<VTXPARTICLE_INSTANCE[]> m_instanceVertices;
	unique_ptr<_float[]> m_speeds;
	_bool m_isLoop = { true };
	_float3 m_range = {};
	_float3 m_pivot = {};
	_float2 m_speedRange;
	_float2 m_lifeTimeRange;
	_float2 m_sizeRange = {};
	_float3 m_angle = {  };


	//_bool m_isKeep = {true};

	_bool m_isFinished = {};
public:
	static shared_ptr<CVIBuffer_Particle_Point> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::InstanceDesc* pParticleDesc);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;


};

NS_END