#pragma once
#include "Bounding.h"
NS_BEGIN(Engine)
class CBounding_Sphere final:
    public CBounding
{

public:
	struct BoundingSphereDesc : public BoundingDesc
	{
		_float		radius;
	};


private:
	CBounding_Sphere(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
	virtual ~CBounding_Sphere() = default;

public:
	HRESULT Initialize(BoundingDesc* desc) override;
	virtual void Update(_fmatrix worldMatrix) override ;
	virtual _bool Intersect(shared_ptr<CBounding> target) override;

	shared_ptr<BoundingSphere> Get_OriginalDesc() {
		return m_originalDesc;
	}
	const shared_ptr<BoundingSphere>Get_Desc() const {
		return m_desc;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* batch, _fvector color);
#endif
protected:


private:
	shared_ptr<BoundingSphere> m_originalDesc = {};
	shared_ptr < BoundingSphere> m_desc = {};

public:
	static shared_ptr<CBounding_Sphere> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, BoundingDesc* desc);
	virtual void Free() override;

};

NS_END