#pragma once
#include "Component.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
NS_BEGIN(Engine)

class ENGINE_DLL CCollider :
    public CComponent
{
private:
	CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Update(_fmatrix WorldMatrix);
	_bool Intersect(shared_ptr<CCollider> pTarget);

#ifdef _DEBUG
	HRESULT Render();
#endif

private:
	COLLIDER				m_eType = { COLLIDER::END };
	shared_ptr<class CBounding> m_pBounding = { nullptr };
	_bool					m_isColl = { false };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ComPtr<ID3D11InputLayout> m_pInputLayout = { nullptr };

#endif
public:
	static shared_ptr<CCollider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, COLLIDER eType);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;


};

NS_END