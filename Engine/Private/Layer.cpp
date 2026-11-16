#include "Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(shared_ptr<CGameObject> gameObject)
{
	if (nullptr == gameObject)
	{
		MSG_BOX("Failed to Add to Layer : GameObject");
		return E_FAIL;
	}

	m_gameObject.push_back(gameObject);

	return S_OK;
}

void CLayer::Priority_Update(_float timeDelta)
{
	for (auto& gameObject : m_gameObject)
	{
		if (nullptr != gameObject)
			gameObject->Priority_Update(timeDelta);
	}
}

void CLayer::Update(_float timeDelta)
{
	for (auto& gameObject : m_gameObject)
	{
		if (nullptr != gameObject)
			gameObject->Update(timeDelta);
	}
}

void CLayer::Late_Update(_float timeDelta)
{
	for (auto it = m_gameObject.begin(); it != m_gameObject.end(); )
	{
		if(nullptr != *it)
			(*it)->Late_Update(timeDelta);

		if ((*it)->Is_PendingDestroy()==true)
		{
			it = m_gameObject.erase(it);
		}
		else
			it++;

	}
}

void CLayer::Update_Gui()
{
	for (auto& gameObject : m_gameObject)
	{
		if (nullptr != gameObject)
			gameObject->OnGui();
	}
}

shared_ptr<CGameObject> CLayer::Get_GameObject(_uint gameObjIndex)
{
	_uint index = {};
	for (auto& obj : m_gameObject)
	{
		if (index == gameObjIndex)
		{
			
			return obj;
		}
		++index;
	}
	return nullptr;
}

list <shared_ptr<class CGameObject>> CLayer::Get_GameObject()
{
	return m_gameObject;
}

shared_ptr<CLayer> CLayer::Create()
{
	shared_ptr<CLayer> instance(new CLayer());
	return instance;
}

void CLayer::Free()
{
	__super::Free();

	//for (auto& pGameObject : m_pGameObject)
	//{
	//	Safe_Release(pGameObject);
	//}
	m_gameObject.clear();
}
