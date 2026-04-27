#pragma once
#include "VIBuffer_Instance.h"


NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Point :
	public CVIBuffer_Instance
{

public:
	struct PARTICLE_POINT_DESC : public CVIBuffer_Instance::INSTANCE_DESC
	{

		_float3	vPivot;
		_float2 vSpeed;
		_float2 vLifeTime;
		_float2 vSizeRange;
		_float3 vRange;
		_bool isLoop;
		_float3 Angle;

	};

protected:
	CVIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype);
public:
	virtual ~CVIBuffer_Particle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;
public:
	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);
	void Spawn(_float fTimeDelta);

	void Stop() { m_isLoop = false; };
	void Start() { m_isLoop = true; };

	void Set_Pivot(const _float3& pivot) { m_Pivot = pivot; }
	_bool Is_Finished() { return m_bFinished; };
protected:

protected:
	unique_ptr<VTXPARTICLE_INSTANCE[]> m_pInstanceVertices;
	unique_ptr<_float[]> m_pSpeeds;
	_bool m_isLoop = { true };
	_float3 m_Range = {};
	_float3 m_Pivot = {};
	_float2 m_SpeedRange;
	_float2 m_LifeTimeRange;
	_float2 m_SizeRange = {};
	_float3 m_Angle = {  };


	//_bool m_isKeep = {true};

	_bool m_bFinished = {};
public:
	static shared_ptr<CVIBuffer_Particle_Point> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;


};

NS_END