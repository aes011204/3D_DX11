#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CShader;

	class ENGINE_DLL CVIBuffer_Sea :
	public CVIBuffer
{
protected:
	CVIBuffer_Sea(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint gridSize, _uint NumLevel);
	CVIBuffer_Sea(const CVIBuffer_Sea& Prototype);
public:
	virtual ~CVIBuffer_Sea() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render(shared_ptr<CShader> ShaderCom);
	
	_float3* Get_VtxPos() { return m_VtxPos; };
private:
	
	_uint m_iNumRingIndices = {};
	_uint m_iNumCenterIndices = {};

	_uint m_NumLevel = {4};
	_uint m_GridSize = { 64 };// 무조건 2의 제곱

	_float3* m_VtxPos = {};
public:
	static shared_ptr<CVIBuffer_Sea> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,
		_uint gridSize, _uint NumLevel);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END