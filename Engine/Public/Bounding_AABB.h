#pragma once
#include "Bounding.h"
NS_BEGIN(Engine)
class CBounding_AABB final:
    public CBounding
{

public:
	struct BoundingAABBDesc : public BoundingDesc
	{
		_float3 extents;
	};


private:
	CBounding_AABB(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
	virtual ~CBounding_AABB() = default;

public:
	HRESULT Initialize(BoundingDesc* desc) override;
	virtual void Update(_fmatrix worldMatrix) override ;
	virtual _bool Intersect(shared_ptr<CBounding> target) override;
	shared_ptr<BoundingBox> Get_OriginalDesc() {
		return m_originalDesc;
	}
	const shared_ptr<BoundingBox>Get_Desc() const{
		return m_desc;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* batch, _fvector color);
#endif
protected:

private:
	shared_ptr<BoundingBox> m_originalDesc = {};
	shared_ptr < BoundingBox> m_desc = {};

public:
	static shared_ptr<CBounding_AABB> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, BoundingDesc* desc);
	virtual void Free() override;

};

NS_END