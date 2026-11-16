#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CShader;

class ENGINE_DLL CVIBuffer_Sea :
	public CVIBuffer
{
protected:
	CVIBuffer_Sea(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, _uint gridSize, _uint numLevel);
	CVIBuffer_Sea(const CVIBuffer_Sea& prototype);
public:
	virtual ~CVIBuffer_Sea() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* arg);
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render(shared_ptr<CShader> shaderCom);

	_float3* Get_VtxPos() { return m_vtxPos; };
	_float3 Snapping(float posY);
private:

	_uint m_numRingIndices = {};
	_uint m_numCenterIndices = {};

	_uint m_numLevel = { 4 };
	_uint m_gridSize = { 64 };// 무조건 2의 제곱

	_float3* m_vtxPos = {};


public:
	static shared_ptr<CVIBuffer_Sea> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
		_uint gridSize, _uint numLevel);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;
};

NS_END