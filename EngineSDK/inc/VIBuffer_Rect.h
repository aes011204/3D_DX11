#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Rect :
    public CVIBuffer
{
protected:
	CVIBuffer_Rect(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CVIBuffer_Rect(const CVIBuffer_Rect& prototype);
public:
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* arg);

public:
	static shared_ptr<CVIBuffer_Rect> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;
};

NS_END