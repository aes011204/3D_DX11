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
	virtual HRESULT Initialize_Prototype(EColliderType eType);
	virtual HRESULT Initialize(void* pArg) override;
	

	void Set_MyLayer(CollisionMask myLayer) { m_myLayer = myLayer; }
	void Set_OtherLayerMask(CollisionMask otherLayer) { m_otherMask = otherLayer; }
	CollisionMask Get_MyLayer() const { return m_myLayer; }
	CollisionMask Get_OtherLayerMask() const { return m_otherMask; }
	const bool Get_CollisionActive() const { return m_isCollisionActive; }
	void Set_CollisionActive(_bool isActive) { m_isCollisionActive = isActive; }

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

	virtual HRESULT Render() override;
#endif

private:
	EColliderType				m_type = { EColliderType::END };
	shared_ptr<class CBounding> m_bounding = { nullptr };
	_bool					m_isColl = { false };

	CollisionMask m_myLayer;
	CollisionMask m_otherMask;

	_bool m_isCollisionActive = { true };


	_bool isOverlap = { false };
	set<weak_ptr<CCollider>, owner_less<>> m_overlapColl;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_batch = { nullptr };
	BasicEffect* m_effect = { nullptr };
	ComPtr<ID3D11InputLayout> m_inputLayout = { nullptr };

#endif
public:
	static shared_ptr<CCollider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, EColliderType eType);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;


};

NS_END