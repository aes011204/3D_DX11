#include "Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(shared_ptr<CGameObject> pGameObject)
{
	if (nullptr == pGameObject)
	{
		MSG_BOX("Failed to Add to Layer : GameObject");
		return E_FAIL;
	}

	m_pGameObject.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_pGameObject)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);
	}
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_pGameObject)
	{
		if (nullptr != pGameObject)
			pGameObject->Update(fTimeDelta);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto it = m_pGameObject.begin(); it != m_pGameObject.end(); )
	{
		if(nullptr != *it)
			(*it)->Late_Update(fTimeDelta);

		if ((*it)->Is_PendingDestroy()==true)
		{
			it = m_pGameObject.erase(it);
		}
		else
			it++;

	}
}

void CLayer::Update_Gui()
{
	for (auto& pGameObject : m_pGameObject)
	{
		if (nullptr != pGameObject)
			pGameObject->OnGui();
	}
}

shared_ptr<CGameObject> CLayer::Get_GameObject(_uint GObjIndex)
{
	_uint index = {};
	for (auto& obj : m_pGameObject)
	{
		if (index == GObjIndex)
		{
			
			return obj;
		}
		++index;
	}
	return nullptr;
}

list <shared_ptr<class CGameObject>> CLayer::Get_GameObject()
{
	return m_pGameObject;
}

shared_ptr<CLayer> CLayer::Create()
{
	shared_ptr<CLayer> pInstance(new CLayer());
	return pInstance;
}

void CLayer::Free()
{
	__super::Free();

	//for (auto& pGameObject : m_pGameObject)
	//{
	//	Safe_Release(pGameObject);
	//}
	m_pGameObject.clear();
}
