#pragma once
#include "VIBuffer_Instance.h"


NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Rect :
	public CVIBuffer_Instance
{

public:
	struct PARTICLE_RECT_DESC : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float2 vSpeed;
		_float2 vLifeTime;
		_bool isLoop;
	};

protected:
	CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype);
public:
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc);
	virtual HRESULT Initialize(void* pArg);

public:
	void Drop(_float fTimeDelta);
	void Spread(_float fTimeDelta);
protected:

protected:
	unique_ptr<VTXPARTICLE_INSTANCE[]> m_pInstanceVertices;
	unique_ptr<_float[]> m_pSpeeds;
	_bool m_isLoop = { false };

public:
	static shared_ptr<CVIBuffer_Particle_Rect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const CVIBuffer_Instance::INSTANCE_DESC* pParticleDesc);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;


};

NS_END