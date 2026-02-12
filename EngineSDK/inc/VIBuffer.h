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
	ComPtr<ID3D11Buffer> m_pVB = { nullptr };
	ComPtr<ID3D11Buffer> m_pIB = { nullptr };

protected:
	_uint						m_iNumVertexBuffers = {};

	_uint						m_iVertexStride = {};
	_uint						m_iNumVertices = {};

	_uint						m_iIndexStride = {};
	_uint						m_iNumIndices = {};

	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveType = {}; // 이건 단순 열거형 자료형이라 comptr 안해도 됨 안되기도 하고 


public:
	virtual shared_ptr<CComponent> Clone(void* pArg)=0;
	virtual void Free() override;
};

NS_END