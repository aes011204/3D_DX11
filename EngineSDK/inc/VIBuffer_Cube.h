#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Cube :
	public CVIBuffer
{
protected:
	CVIBuffer_Cube(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CVIBuffer_Cube(const CVIBuffer_Cube& prototype);
public:
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* arg);

public:
	static shared_ptr<CVIBuffer_Cube> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;
};

NS_END
