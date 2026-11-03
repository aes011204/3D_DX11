#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CVIBuffer abstract:
    public CComponent
{
protected:
	CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer(const CVIBuffer& Prototype);
public:
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Buffer> m_VB = { nullptr };
	ComPtr<ID3D11Buffer> m_IB = { nullptr };

protected:
	_uint						m_numVertexBuffers = {};

	_uint						m_vertexStride = {};
	_uint						m_numVertices = {};

	_uint						m_indexStride = {};
	_uint						m_numIndices = {};

	D3D11_PRIMITIVE_TOPOLOGY	m_primitiveType = {}; // 이건 단순 열거형 자료형이라 comptr 안해도 됨 안되기도 하고 


public:
	virtual shared_ptr<CComponent> Clone(void* pArg)=0;
	virtual void Free() override;
};

NS_END