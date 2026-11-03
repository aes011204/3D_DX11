#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CBounding abstract:
    public CBase
{
public:
    struct BoundingDesc
    {
        _float3 center;

		CollisionMask myLayer = {0};
		CollisionMask otherMask = {0};
    };


protected:
	CBounding(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
	virtual ~CBounding() = default;

public:
	
	virtual HRESULT Initialize(BoundingDesc* desc);
	virtual void Update(_fmatrix worldMatrix)=0;
	virtual _bool Intersect(shared_ptr<CBounding> target)=0;




#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* batch, _fvector color)=0;



#endif
protected:
	ComPtr<ID3D11Device> m_device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_context = { nullptr };

public:
	virtual void Free() override;
};

NS_END