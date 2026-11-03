#include "Object_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
	: m_gameInstance(CGameInstance::GetInstance())
{


}

CObject_Manager::~CObject_Manager()
{
	Free();
}

HRESULT CObject_Manager::Initialize(_uint numLevels)
{
	m_numLevel = numLevels;
	m_layers = new LAYERS[numLevels];
	return S_OK;
}

shared_ptr<CGameObject> CObject_Manager::Add_GameObject(_uint prototypeLevelIndex, const _wstring& prototypeTag,
	_uint layerLevelIndex, const _wstring& layerTag, void* arg)
{
	shared_ptr<CGameObject> gameObject = { nullptr };
	shared_ptr<CLayer> layer = { nullptr };

	if (nullptr == m_layers ||
		layerLevelIndex >= m_numLevel)
		goto except;

 	gameObject = dynamic_pointer_cast<CGameObject>(m_gameInstance.lock()->Clone_Prototype(EPrototype::GAMEOBJECT, prototypeLevelIndex, prototypeTag, arg));
	if (nullptr == gameObject)
		goto except;

	layer = Find_Layer(layerLevelIndex, layerTag);

	if (nullptr == layer)
	{
		layer = CLayer::Create();
		layer->Add_GameObject(gameObject);
		m_layers[layerLevelIndex].emplace(layerTag, layer);
	}
	else
		layer->Add_GameObject(gameObject);


	return gameObject;

except:
	MSG_BOX("Failed To Added : CloneGameObject");
	return nullptr;
}

shared_ptr<CGameObject> CObject_Manager::Get_GameObject(_uint levelIndex,const _wstring& layerTag, _uint gameObjIndex)
{
	shared_ptr<CLayer> layer = Find_Layer(levelIndex, layerTag);
	if (layer == nullptr)
	{
		return nullptr;
	}

		return layer->Get_GameObject(gameObjIndex);
}



void CObject_Manager::Priority_Update(_float timeDelta)
{
	// 동적배열로 선언한건 범위기반 for 불가능
	for(_uint i=0; i<m_numLevel;i++)
	{
		for(auto& pair : m_layers[i])
		{
			if(pair.second != nullptr)
				pair.second->Priority_Update(timeDelta);
		}
	}
}

void CObject_Manager::Update(_float timeDelta)
{
	// 동적배열로 선언한건 범위기반 for 불가능
	for (_uint i = 0; i < m_numLevel;i++)
	{
		for (auto& pair : m_layers[i])
		{
			if (pair.second != nullptr)
				pair.second->Update(timeDelta);
		}
	}

}

void CObject_Manager::Late_Update(_float timeDelta)
{
	// 동적배열로 선언한건 범위기반 for 불가능
	for (_uint i = 0; i < m_numLevel;i++)
	{
		for (auto& pair : m_layers[i])
		{
			if (pair.second != nullptr)
				pair.second->Late_Update(timeDelta);

		}
	}
}

HRESULT CObject_Manager::Clear_Layers(_uint levelIndex)
{
	
	
	if (m_numLevel <= levelIndex)
		return E_FAIL;

	m_layers[levelIndex].clear();

	return S_OK;
}

void CObject_Manager::Update_Gui()
{
	for (_uint i = 0; i < m_numLevel;i++)
	{
		for (auto& pair : m_layers[i])
		{
			if (pair.second != nullptr)
				pair.second->Update_Gui();
		}
	}
}

const map<const _wstring, shared_ptr<CLayer>>& CObject_Manager::Get_GameObjects(_uint levelIndex) const
{
	return m_layers[levelIndex];

}

shared_ptr<CLayer> CObject_Manager::Find_Layer(_uint levelIndex, const _wstring& layerTag)
{
	if (m_numLevel <= levelIndex)
		return nullptr;

	auto it = m_layers[levelIndex].find(layerTag);
	// find() : 객체 반환
	if (it == m_layers[levelIndex].end())
		return nullptr;

	return it->second;

}

unique_ptr<CObject_Manager> CObject_Manager::Create(_uint numLevels)
{
	unique_ptr<CObject_Manager> instance(new CObject_Manager());

	if (FAILED(instance->Initialize(numLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
	}
	return instance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_numLevel;i++)
	{
		m_layers[i].clear();
	}
	Safe_Delete_Array(m_layers); // 배열지우는 매크로 


}
