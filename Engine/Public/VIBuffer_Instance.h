#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance abstract:
    public CVIBuffer
{

public:
	struct INSTANCE_DESC
	{
		_uint		iNumInstances = {};
		_float3		vCenter;
		_float3		vRange;
		_float2		vScale;
	};

protected:
	CVIBuffer_Instance(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& Prototype);
public:
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Buffer>  m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC m_InstanceBufferDesc{};

protected:
	_uint						m_iInstanceStride = {};

	_uint						m_iNumInstances = {};
	_uint						m_iIndexCountPerInstance = {};

	
public:
	virtual shared_ptr<CComponent> Clone(void* pArg)=0;
	virtual void Free() override;


};

NS_END