#pragma once
#include "VIBuffer_Instance.h"


NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Rect :
	public CVIBuffer_Instance
{

public:
	struct ParticleRectDesc : public CVIBuffer_Instance::InstanceDesc
	{
		_float2 speed;
		_float2 lifeTime;
		_bool isLoop;
	};

protected:
	CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& prototype);
public:
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instance::InstanceDesc* particleDesc);
	virtual HRESULT Initialize(void* arg);

public:
	void Drop(_float timeDelta);
	void Spread(_float timeDelta);
protected:

protected:
	unique_ptr<VTXPARTICLE_INSTANCE[]> m_instanceVertices;
	unique_ptr<_float[]> m_speeds;
	_bool m_isLoop = { false };

public:
	static shared_ptr<CVIBuffer_Particle_Rect> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const CVIBuffer_Instance::InstanceDesc* particleDesc);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;


};

NS_END