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
	CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype);
public:
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instance::InstanceDesc* pParticleDesc);
	virtual HRESULT Initialize(void* pArg);

public:
	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);
protected:

protected:
	unique_ptr<VTXPARTICLE_INSTANCE[]> m_instanceVertices;
	unique_ptr<_float[]> m_speeds;
	_bool m_isLoop = { false };

public:
	static shared_ptr<CVIBuffer_Particle_Rect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::InstanceDesc* pParticleDesc);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;


};

NS_END