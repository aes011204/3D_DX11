#pragma once
#include "Bounding.h"
NS_BEGIN(Engine)
class CBounding_AABB final:
    public CBounding
{

public:
	struct BOUNDING_AABB_DESC : public BOUNDING_DESC
	{
		_float3 vExtents;
	};


private:
	CBounding_AABB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CBounding_AABB() = default;

public:
	HRESULT Initialize(BOUNDING_DESC* pDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override ;
	virtual _bool Intersect(shared_ptr<CBounding> pTarget) override;
	shared_ptr<BoundingBox> Get_OriginalDesc() {
		return m_pOriginalDesc;
	}
	const shared_ptr<BoundingBox>Get_Desc() const{
		return m_pDesc;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor);
#endif
protected:

private:
	shared_ptr<BoundingBox> m_pOriginalDesc = {};
	shared_ptr < BoundingBox> m_pDesc = {};

public:
	static shared_ptr<CBounding_AABB> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

NS_END