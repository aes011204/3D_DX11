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

	void Set_MyLayer(COLLISION_MASK myLayer) { m_MyLayer = myLayer; }
	void Set_OtherLayerMask(COLLISION_MASK otherLayer) { m_OtherMask = otherLayer; }
	COLLISION_MASK Get_MyLayer() const { return m_MyLayer; }
	COLLISION_MASK Get_OtherLayerMask() const { return m_OtherMask; }
	const bool Get_CollisionActive() const { return m_IsCollisionActive; }
	void Set_CollisionActive(_bool isActive) { m_IsCollisionActive = isActive; }

	void Set_IsColl(bool isColl) { m_isColl = isColl; }

	void Set_isOverlap(){};

	void OnGui() override;
	void Save_ToJson(nlohmann::json& j) override;
	void Load_FromJson(nlohmann::json& j) override;

public:
	void Update(_fmatrix WorldMatrix);
	_bool Intersect(shared_ptr<CCollider> pTarget);

	bool Is_Overlap(shared_ptr<CCollider> other);
	void Add_Overlap(shared_ptr<CCollider> otherColl){m_overlapColl.insert(otherColl);};
	void Remove_Overlap(shared_ptr<CCollider> otherColl){ m_overlapColl.erase(otherColl); };



	_float3 Get_WorldCenter();

#ifdef _DEBUG
	HRESULT Render();
#endif

private:
	COLLIDER				m_eType = { COLLIDER::END };
	shared_ptr<class CBounding> m_pBounding = { nullptr };
	_bool					m_isColl = { false };

	COLLISION_MASK m_MyLayer;
	COLLISION_MASK m_OtherMask;

	_bool m_IsCollisionActive = { true };


	_bool isOverlap = { false };
	set<weak_ptr<CCollider>, owner_less<>> m_overlapColl;

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