#pragma once
#include "Bounding.h"
NS_BEGIN(Engine)
class CBounding_Sphere final:
    public CBounding
{

public:
	struct BOUNDING_SPHERE_DESC : public BOUNDING_DESC
	{
		_float		fRadius;
	};


private:
	CBounding_Sphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CBounding_Sphere() = default;

public:
	HRESULT Initialize(BOUNDING_DESC* pDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override ;
	virtual _bool Intersect(shared_ptr<CBounding> pTarget) override;

	const shared_ptr<BoundingSphere> Get_Desc() const {
		return m_pDesc;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor);
#endif
protected:


private:
	shared_ptr<BoundingSphere> m_pOriginalDesc = {};
	shared_ptr < BoundingSphere> m_pDesc = {};

public:
	static shared_ptr<CBounding_Sphere> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

NS_END