#pragma once
#include "Bounding.h"
NS_BEGIN(Engine)
class CBounding_OBB final:
    public CBounding
{

public:
	struct BoundingOBBDesc : public BoundingDesc
	{
		_float3 extents;
		_float3 radians;
	};


private:
	CBounding_OBB(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
	virtual ~CBounding_OBB() = default;

public:
	HRESULT Initialize(BoundingDesc* desc) override;
	virtual void Update(_fmatrix worldMatrix) override ;
	virtual _bool Intersect(shared_ptr<CBounding> target) override;

	shared_ptr<BoundingOrientedBox> Get_OriginalDesc() {
		return m_originalDesc;
	}
	const shared_ptr<BoundingOrientedBox>Get_Desc() const {
		return m_desc;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* batch, _fvector color);
#endif
protected:

private:
	shared_ptr<BoundingOrientedBox> m_originalDesc = {};
	shared_ptr < BoundingOrientedBox> m_desc = {};

public:
	static shared_ptr<CBounding_OBB> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, BoundingDesc* desc);
	virtual void Free() override;

};

NS_END