#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance abstract:
    public CVIBuffer
{

public:
	struct InstanceDesc
	{
		_uint		numInstances = {};
		_float3		center;
		_float3		range;
		_float2		scale;
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
	ComPtr<ID3D11Buffer>  m_VBInstance = { nullptr };
	D3D11_BUFFER_DESC m_instanceBufferDesc{};

protected:
	_uint						m_instanceStride = {};

	_uint						m_numInstances = {};
	_uint						m_indexCountPerInstance = {};

	
public:
	virtual shared_ptr<CComponent> Clone(void* pArg)=0;
	virtual void Free() override;


};

NS_END