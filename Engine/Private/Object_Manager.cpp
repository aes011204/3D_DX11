#include "Object_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{


}

CObject_Manager::~CObject_Manager()
{
	Free();
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevel = iNumLevels;
	m_pLayers = new LAYERS[iNumLevels];
	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
	_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	shared_ptr<CGameObject> pGameObject = { nullptr };
	shared_ptr<CLayer> pLayer = { nullptr };

	if (nullptr == m_pLayers ||
		iLayerLevelIndex >= m_iNumLevel)
		goto except;

	pGameObject = dynamic_pointer_cast<CGameObject>(m_pGameInstance.lock()->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		goto except;

	pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);


	return S_OK;

except:
	MSG_BOX("Failed To Added : CloneGameObject");
	return E_FAIL;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	// 동적배열로 선언한건 범위기반 for 불가능
	for(int i=0; i<m_iNumLevel;i++)
	{
		for(auto& pair : m_pLayers[i])
		{
			if(pair.second != nullptr)
				pair.second->Priority_Update(fTimeDelta);
		}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	// 동적배열로 선언한건 범위기반 for 불가능
	for (int i = 0; i < m_iNumLevel;i++)
	{
		for (auto& pair : m_pLayers[i])
		{
			if (pair.second != nullptr)
				pair.second->Update(fTimeDelta);
		}
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	// 동적배열로 선언한건 범위기반 for 불가능
	for (int i = 0; i < m_iNumLevel;i++)
	{
		for (auto& pair : m_pLayers[i])
		{
			if (pair.second != nullptr)
				pair.second->Late_Update(fTimeDelta);
		}
	}
}

HRESULT CObject_Manager::Clear_Layers(_uint iLevelIndex)
{
	if (m_iNumLevel <= iLevelIndex)
		return E_FAIL;

	//for(auto& pair :m_pLayers[iLevelIndex])
	//{
	//	Safe_Release(pair.second);
	//}
	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

void CObject_Manager::Update_Gui()
{
	for (int i = 0; i < m_iNumLevel;i++)
	{
		for (auto& pair : m_pLayers[i])
		{
			if (pair.second != nullptr)
				pair.second->Update_Gui();
		}
	}
}

map<const _wstring, shared_ptr<CLayer>> CObject_Manager::Get_GameObjects(_uint levelIndex)
{
	return m_pLayers[levelIndex];

}

shared_ptr<CLayer> CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	if (m_iNumLevel <= iLevelIndex)
		return nullptr;

	auto it = m_pLayers[iLevelIndex].find(strLayerTag);
	// find() : 객체 반환
	if (it == m_pLayers[iLevelIndex].end())
		return nullptr;

	return it->second;

}

unique_ptr<CObject_Manager> CObject_Manager::Create(_uint iNumLevels)
{
	unique_ptr<CObject_Manager> pInstance(new CObject_Manager());

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
	}
	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (int i = 0; i < m_iNumLevel;i++)
	{
		//for (auto& pair : m_pLayers[i])
		//{
		//	Safe_Release(pair.second);
		//}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers); // 배열지우는 매크로 



}
