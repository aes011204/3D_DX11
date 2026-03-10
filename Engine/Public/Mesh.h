#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)

class CMesh final:
    public CVIBuffer
{
private:
	CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMesh(const CMesh& Prototype);
public:
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);


public:
	static shared_ptr<CMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END