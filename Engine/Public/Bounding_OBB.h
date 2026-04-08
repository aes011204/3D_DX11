#pragma once
#include "Bounding.h"
NS_BEGIN(Engine)
class CBounding_OBB final:
    public CBounding
{

public:
	struct BOUNDING_OBB_DESC : public BOUNDING_DESC
	{
		_float3 vExtents;
		_float3 vRadians;
	};


private:
	CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CBounding_OBB() = default;

public:
	HRESULT Initialize(BOUNDING_DESC* pDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override ;
	virtual _bool Intersect(shared_ptr<CBounding> pTarget) override;

	shared_ptr<BoundingOrientedBox> Get_OriginalDesc() {
		return m_pOriginalDesc;
	}
	const shared_ptr<BoundingOrientedBox>Get_Desc() const {
		return m_pDesc;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor);
#endif
protected:

private:
	shared_ptr<BoundingOrientedBox> m_pOriginalDesc = {};
	shared_ptr < BoundingOrientedBox> m_pDesc = {};

public:
	static shared_ptr<CBounding_OBB> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

NS_END