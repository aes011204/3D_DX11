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
	CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CBounding_OBB() = default;

public:
	HRESULT Initialize(BoundingDesc* pDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override ;
	virtual _bool Intersect(shared_ptr<CBounding> pTarget) override;

	shared_ptr<BoundingOrientedBox> Get_OriginalDesc() {
		return m_originalDesc;
	}
	const shared_ptr<BoundingOrientedBox>Get_Desc() const {
		return m_desc;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor);
#endif
protected:

private:
	shared_ptr<BoundingOrientedBox> m_originalDesc = {};
	shared_ptr < BoundingOrientedBox> m_desc = {};

public:
	static shared_ptr<CBounding_OBB> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, BoundingDesc* pDesc);
	virtual void Free() override;

};

NS_END