#include "Prototype_Manager.h"

#include "Component.h"
#include "GameInstance.h"
#include "EventBus.h"
#include "GameObject.h"
#include "Engine_Helper.h"
#include "Event_Struct.h"
#include "DInput_Manager.h"


CPrototype_Manager::CPrototype_Manager() :
	m_gameInstance{ CGameInstance::GetInstance() }
{
}


CPrototype_Manager::~CPrototype_Manager()
{
	Free();
}

HRESULT CPrototype_Manager::Initialize(_uint numLevels)
{
	m_numLevel = numLevels;

	m_prototypes = new PROTOTYPES[numLevels];

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint levelIndex, const _wstring& prototypeTag, shared_ptr<CBase> prototype)
{
	if (levelIndex >= m_numLevel) // 배열 인댁스라서 
		return E_FAIL;

	//// 같은 키를 가진 객체가 있는지 확인
	//if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
	//{
	//    MSG_BOX("An object with the same key already exists.");
	//    return E_FAIL;
	//}

	//m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
	auto result = m_prototypes[levelIndex].insert(make_pair(prototypeTag, prototype));

	prototype->Set_ProtoLevel(levelIndex);
	prototype->Set_ProtoTag(prototypeTag);


	if (result.second == false) {
		// 이미 키가 있거나 삽입에 실패한 경우
		MSG_BOX("Insert Failed!");
	}


	return S_OK;
}

shared_ptr<CBase> CPrototype_Manager::Clone_Prototype(EPrototype prototy, _uint levelIndex, const _wstring& prototypeTag, void* arg)
{
	/* 조건에 맞는 원형객체를 검색한다. */
	shared_ptr<CBase> prototype = Find_Prototype(levelIndex, prototypeTag);
	if (nullptr == prototype)
		return nullptr;



	if (prototy == EPrototype::GAMEOBJECT)
	{
		shared_ptr<CGameObject> instance = { nullptr };
		shared_ptr<CGameObject> origin = dynamic_pointer_cast<CGameObject>(prototype);
		instance = origin->Clone(arg);

		if (instance == nullptr)
		{
			MSG_BOX("Failed to Cloned");
			return nullptr;
		}

		instance->Set_ProtoTag(origin->Get_ProtoTag());
		instance->Set_ProtoLevel(origin->Get_ProtoLevel());

		return instance;

	}
	else
	{
		shared_ptr<CComponent> instance = { nullptr };
		shared_ptr<CComponent> origin = dynamic_pointer_cast<CComponent>(prototype);
		instance = origin->Clone(arg);

		if (instance == nullptr)
		{
			MSG_BOX("Failed to Cloned");
			return nullptr;
		}
		string str = W2S(origin->Get_ProtoTag());
		instance->Set_ProtoTag(origin->Get_ProtoTag());
		instance->Set_ProtoLevel(origin->Get_ProtoLevel());
		string str22 = W2S(instance->Get_ProtoTag());

		return instance;

	}

	/*  if(pInstance == nullptr)
	  {
		  MSG_BOX("Failed to Cloned");
		  return nullptr;
	  }*/

	MSG_BOX("Failed to Cloned");
	return nullptr;

}

//shared_ptr<CBase> CPrototype_Manager::Clone_Prototype(shared_ptr<CBase> pPrototype, void* pArg)
//{
//    /* 조건에 맞는 원형객체를 검색한다. */
//    //shared_ptr<CBase> pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
//    if (nullptr == pPrototype)
//        return nullptr;
//
//    shared_ptr<CBase> pInstance = { nullptr };
//
//    auto pGameObject = dynamic_pointer_cast<CGameObject>(pPrototype);
//
//    if (pGameObject)
//    {
//        pInstance = pGameObject->Clone(pArg);
//    }
//    else
//    {
//        auto pComponent = dynamic_pointer_cast<CComponent>(pPrototype);
//        if (pComponent)
//        {
//            pInstance = pComponent->Clone(pArg);
//        }
//    }
//
//    // 3. 둘 다 아니거나 클론에 실패한 경우
//    if (pInstance == nullptr)
//    {
//        MSG_BOX("Failed to Cloned");
//        return nullptr;
//    }
//
//    //pInstance->Set_ProtoTag(L"strPrototypeTag");
//    //pInstance->Set_ProtoLevel(iPrototypeLevelIndex);
//
//    return pInstance;
//}

HRESULT CPrototype_Manager::Clear_Prototype(_uint levelIndex)
{
	if (levelIndex >= m_numLevel)
		return E_FAIL;


	m_prototypes[levelIndex].clear();

	return S_OK;

}

void CPrototype_Manager::OnGui()
{
	shared_ptr<CEntity> choseEntt = { nullptr };
	for (_uint i = 0; i < m_numLevel; ++i)
	{
		string levelName = "Level_" + to_string(i);


		if (ImGui::TreeNode(levelName.c_str()))
		{
			auto copyProtoType = m_prototypes;
			for (auto& pair : copyProtoType[i])
			{
				string tagStr = Engine::W2S(pair.first);

				string searchStr = "Prototype_";
				size_t pos = tagStr.find(searchStr);

				if (pos != string::npos)
				{
					// "Prototype_" 이후
					tagStr = tagStr.substr(pos + searchStr.length());
				}

				// 리스트 아이템 출력
				bool isSelected = (m_selectedTag == pair.first && m_selectedLevel == i);



				if (ImGui::Selectable(tagStr.c_str(), isSelected))
				{
					m_selectedTag = pair.first;
					m_selectedLevel = i;
				}
				// 드래그
				if (ImGui::BeginDragDropSource()) {

					DragDropProto protoCloneDesc = {};
					//wcscpy_s(payload.szTag, pair.first.c_str());
					wcscpy_s(protoCloneDesc.tag, pair.first.c_str());
					protoCloneDesc.level = i;
					ImGui::SetDragDropPayload("COMP_DRAG_DATA", &protoCloneDesc, sizeof(DragDropProto));

					ImGui::Text("Swap to %s", Engine::W2S(protoCloneDesc.tag).c_str());
					ImGui::EndDragDropSource();
				}
			}


			ImGui::TreePop();
		}
	}
	ImGui::Separator();

	ImGui::Separator();

	// 추가 버튼 로직 
	if (m_selectedTag != L"")
	{
		string btnLabel = "Add [" + Engine::W2S(m_selectedTag) + "]";

		// 버튼 클릭 시 즉시 실행
		if (ImGui::Button(btnLabel.c_str(), ImVec2(-1, 30)))
		{
			EvtTerrainPicking tmp = {};
			tmp.isOnPicking = true;
			tmp.level =  m_selectedLevel;
			tmp.tag = m_selectedTag;
			m_gameInstance.lock()->Get_EventBus()->Publish(tmp);

		
		}

		
	}
	else
	{
		ImGui::TextDisabled("Select a prototype to add.");
	}

}


shared_ptr<CBase> CPrototype_Manager::Find_Prototype(_uint levelIndex, const _wstring& prototypeTag)
{
	if (levelIndex >= m_numLevel)
		return nullptr;

	auto iter = m_prototypes[levelIndex].find(prototypeTag);
	if (iter == m_prototypes[levelIndex].end())
		return nullptr;

	return iter->second;
}

unique_ptr<CPrototype_Manager> CPrototype_Manager::Create(_uint numLevel)
{
	unique_ptr<CPrototype_Manager> instance(new CPrototype_Manager());

	if (FAILED(instance->Initialize(numLevel)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
	}
	return instance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_numLevel;i++)
	{
		//for(auto& pair : m_pPrototypes[i])
		//{
	 //       Safe_Release(pair.second);
		//}
		m_prototypes[i].clear();
	}
	Safe_Delete_Array(m_prototypes); // 배열지우는 매크로 
}
