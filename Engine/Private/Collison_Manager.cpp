#include "Collison_Manager.h"

#include "GameObject.h"
#include "Collider.h"

CCollision_Manager::CCollision_Manager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
}

CCollision_Manager::~CCollision_Manager()
{
	Free();
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Update()
{

	if (m_colliders.empty())
		return;

	for (size_t i = 0; i < m_colliders.size(); ++i)
	{
		auto collider = m_colliders[i].lock();
		if (collider)
			collider->Set_IsColl(false);
	}



	for (_uint i = 0; i < m_colliders.size(); i++)
	{
		auto src = m_colliders[i].lock();
		if (src == nullptr)
			continue;
		shared_ptr<CGameObject> ownerSrc = src->Get_GOwner();
		if (ownerSrc == nullptr || ownerSrc->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
			continue;
		for(_uint j = 0; j < m_colliders.size(); j++)
		{
			auto dst = m_colliders[j].lock();
			if (dst == nullptr)
				continue;
			shared_ptr<CGameObject> ownerDst = dst->Get_GOwner();
			if (ownerDst == nullptr || ownerDst->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
				continue;

			if (ownerSrc == ownerDst)
				continue;

			if (src->Get_CollisionActive() == false)
				continue;
			if (dst->Get_CollisionActive() == false)
				continue;


			if (Can_Collider(src, dst) == false)
				continue;


			_bool isHit = src->Intersect(dst);

			_bool wasOverlap = src->Is_Overlap(dst);

			if(isHit)
			{
				src->Set_IsColl(true);
				dst->Set_IsColl(true);

				if(!wasOverlap)
				{
					// 최초 충돌
					src->Get_GOwner()->OnBeginOverlap(src, dst);
					dst->Get_GOwner()->OnBeginOverlap(dst,src);

					src->Add_Overlap(dst);
					dst->Add_Overlap(src);

				}
				else
				{
					// 유지
					src->Get_GOwner()->OnStayOverlap(src, dst);
					dst->Get_GOwner()->OnStayOverlap(dst, src);
				}

			}
			else
			{
				if(wasOverlap)
				{
					// 나감

					src->Get_GOwner()->OnEndOverlap(src, dst);
					dst->Get_GOwner()->OnEndOverlap(dst, src);

					src->Remove_Overlap(dst);
					dst->Remove_Overlap(src);
				}
			}
		}
		
	}
}
_bool CCollision_Manager::Can_Collider(shared_ptr<CCollider> src, shared_ptr<CCollider> dst)
{
	if (nullptr == src || nullptr == dst)
		return false;

	if (src == dst)
		return false;

	_bool isSrcCanSeeDst = (src->Get_OtherLayerMask() & dst->Get_MyLayer()) != 0;
	_bool isDstCanSeeSrc = (dst->Get_OtherLayerMask() & src->Get_MyLayer()) != 0;

	return isSrcCanSeeDst && isDstCanSeeSrc;

}
void CCollision_Manager::Add_Collider(shared_ptr<CCollider> coll)
{
	if (coll == nullptr)
		return;
	m_colliders.push_back(coll);
}

void CCollision_Manager::Remove_Collider(shared_ptr<CCollider> coll)
{

}

unique_ptr<CCollision_Manager> CCollision_Manager::Create(ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> context)
{
	unique_ptr<CCollision_Manager> instance(new CCollision_Manager(device, context));

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
	}
	return instance;
}

void CCollision_Manager::Free()
{
	CBase::Free();
}
