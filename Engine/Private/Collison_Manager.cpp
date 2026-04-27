#include "Collison_Manager.h"

#include "GameObject.h"
#include "Collider.h"

CCollision_Manager::CCollision_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
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

	if (m_Colliders.empty())
		return;

	for (size_t i = 0; i < m_Colliders.size(); ++i)
	{
		auto collider = m_Colliders[i].lock();
		if (collider)
			collider->Set_IsColl(false);
	}



	for (_uint i = 0; i < m_Colliders.size(); i++)
	{
		auto Src = m_Colliders[i].lock();
		if (Src == nullptr)
			continue;
		shared_ptr<CGameObject> ownerSrc = Src->Get_GOwner();
		if (ownerSrc == nullptr || ownerSrc->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
			continue;
		for(_uint j = 0; j < m_Colliders.size(); j++)
		{
			auto Dst = m_Colliders[j].lock();
			if (Dst == nullptr)
				continue;
			shared_ptr<CGameObject> ownerDst = Dst->Get_GOwner();
			if (ownerDst == nullptr || ownerDst->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
				continue;

			if (ownerSrc == ownerDst)
				continue;

			if (Src->Get_CollisionActive() == false)
				continue;
			if (Dst->Get_CollisionActive() == false)
				continue;


			if (Can_Collider(Src, Dst) == false)
				continue;


			_bool isHit = Src->Intersect(Dst);

			_bool wasOverlap = Src->Is_Overlap(Dst);

			if(isHit)
			{
				Src->Set_IsColl(true);
				Dst->Set_IsColl(true);

				if(!wasOverlap)
				{
					// 최초 충돌
					Src->Get_GOwner()->OnBeginOverlap(Src, Dst);
					Dst->Get_GOwner()->OnBeginOverlap(Dst,Src);

					Src->Add_Overlap(Dst);
					Dst->Add_Overlap(Src);

				}
				else
				{
					// 유지
					Src->Get_GOwner()->OnStayOverlap(Src, Dst);
					Dst->Get_GOwner()->OnStayOverlap(Dst, Src);
				}

			}
			else
			{
				if(wasOverlap)
				{
					// 나감

					Src->Get_GOwner()->OnEndOverlap(Src, Dst);
					Dst->Get_GOwner()->OnEndOverlap(Dst, Src);

					Src->Remove_Overlap(Dst);
					Dst->Remove_Overlap(Src);
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

	_bool bSrcCanSeeDst = (src->Get_OtherLayerMask() & dst->Get_MyLayer()) != 0;
	_bool bDstCanSeeSrc = (dst->Get_OtherLayerMask() & src->Get_MyLayer()) != 0;

	return bSrcCanSeeDst && bDstCanSeeSrc;

}
void CCollision_Manager::Add_Collider(shared_ptr<CCollider> coll)
{
	if (coll == nullptr)
		return;
	m_Colliders.push_back(coll);
}

void CCollision_Manager::Remove_Collider(shared_ptr<CCollider> coll)
{

}

unique_ptr<CCollision_Manager> CCollision_Manager::Create(ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CCollision_Manager> pInstance(new CCollision_Manager(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
	}
	return pInstance;
}

void CCollision_Manager::Free()
{
	CBase::Free();
}
