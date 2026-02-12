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
	for (auto& pGameObject : m_pGameObject)
	{
		if(nullptr != pGameObject)
			pGameObject->Late_Update(fTimeDelta);
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
