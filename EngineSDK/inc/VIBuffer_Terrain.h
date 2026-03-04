#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Terrain :
    public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
public:
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg);

	_uint Get_NumVerticeX() { return { m_iNumVerticesX }; }
	_uint Get_NumVerticeZ() { return { m_iNumVerticesX }; }
	_float3* Get_VtxPos() { return m_VtxPos; };
private:
	_uint				m_iNumVerticesX = {};
	_uint				m_iNumVerticesZ = {};

	_float3* m_VtxPos = {};
public:
	static shared_ptr<CVIBuffer_Terrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pHeightMapFilePath);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END