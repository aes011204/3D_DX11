#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CBounding abstract:
    public CBase
{
public:
    struct BOUNDING_DESC
    {
        _float3 vCenter;
    };


protected:
	CBounding(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CBounding() = default;

public:
	
	virtual HRESULT Initialize(BOUNDING_DESC* pDesc);
	virtual void Update(_fmatrix WorldMatrix)=0;
	virtual _bool Intersect(shared_ptr<CBounding> pTarget)=0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)=0;
#endif
protected:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
public:
	virtual void Free() override;
};

NS_END