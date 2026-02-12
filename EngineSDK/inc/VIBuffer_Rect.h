#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect :
    public CVIBuffer
{
protected:
	CVIBuffer_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& Prototype);
public:
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static shared_ptr<CVIBuffer_Rect> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END