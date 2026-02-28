#include "Prototype_Manager.h"

#include "Component.h"
#include "GameInstance.h"
#include "EventBus.h"
#include "GameObject.h"
#include "Engine_Helper.h"
#include "Event_Struct.h"


CPrototype_Manager::CPrototype_Manager(): 
	m_pGameInstance { CGameInstance::GetInstance() }
{
}


CPrototype_Manager::~CPrototype_Manager()
{
	Free();
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevel = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, shared_ptr<CBase> pPrototype)
{
	if (iLevelIndex >= m_iNumLevel) // 배열 인댁스라서 
		return E_FAIL;

	//// 같은 키를 가진 객체가 있는지 확인
	//if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
	//{
	//    MSG_BOX("An object with the same key already exists.");
	//    return E_FAIL;
	//}

	//m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
	auto result = m_pPrototypes[iLevelIndex].insert(make_pair(strPrototypeTag, pPrototype));

	pPrototype->Set_ProtoLevel(iLevelIndex);
	pPrototype->Set_ProtoTag(strPrototypeTag);


	if (result.second == false) {
		// 이미 키가 있거나 삽입에 실패한 경우
		MSG_BOX("Insert Failed!");
	}


	return S_OK;
}

shared_ptr<CBase> CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototy, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	/* 조건에 맞는 원형객체를 검색한다. */
	shared_ptr<CBase> pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;



	if (ePrototy == PROTOTYPE::GAMEOBJECT)
	{
		shared_ptr<CGameObject> pInstance = { nullptr };
		shared_ptr<CGameObject> origin = dynamic_pointer_cast<CGameObject>(pPrototype);
		pInstance = origin->Clone(pArg);

		if (pInstance == nullptr)
		{
			MSG_BOX("Failed to Cloned");
			return nullptr;
		}

		pInstance->Set_ProtoTag(origin->Get_ProtoTag());
		pInstance->Set_ProtoLevel(origin->Get_ProtoLevel());

		return pInstance;

	}
	else
	{
		shared_ptr<CComponent> pInstance = { nullptr };
		shared_ptr<CComponent> origin = dynamic_pointer_cast<CComponent>(pPrototype);
		pInstance = origin->Clone(pArg);

		if (pInstance == nullptr)
		{
			MSG_BOX("Failed to Cloned");
			return nullptr;
		}
		string str = W2S(origin->Get_ProtoTag());
		pInstance->Set_ProtoTag(origin->Get_ProtoTag());
		pInstance->Set_ProtoLevel(origin->Get_ProtoLevel());
		string str22 = W2S(pInstance->Get_ProtoTag());

		return pInstance;

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

HRESULT CPrototype_Manager::Clear_Prototype(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevel)
		return E_FAIL;


	m_pPrototypes[iLevelIndex].clear();

	return S_OK;

}

void CPrototype_Manager::OnGui()
{
	shared_ptr<CEntity> choseEntt = { nullptr };
	for (_uint i = 0; i < m_iNumLevel; ++i)
	{
		string levelName = "Level_" + to_string(i);

		// 1. 레벨 트리 노드 시작
		if (ImGui::TreeNode(levelName.c_str()))
		{
			auto CopyProtoType = m_pPrototypes;
			for (auto& pair : CopyProtoType[i])
			{
				string tagStr = Engine::W2S(pair.first);

				string searchStr = "Prototype_";
				size_t pos = tagStr.find(searchStr);

				if (pos != string::npos)
				{
					// "Prototype_" 이후의 문자열만 취함
					tagStr = tagStr.substr(pos + searchStr.length());
				}

				// 리스트 아이템 출력
				bool isSelected = (m_strSelectedTag == pair.first && m_iSelectedLevel == i);

				//bool isSelected = CEditorInstance::GetCurSelect() == pair.second;
				//if (ImGui::Selectable(tagStr.c_str(), isSelected))
				//{
				//    //m_strSelectedTag = pair.first;
				//    ////choseEntt = dynamic_pointer_cast<CEntity>(pair.second);
				//    //m_iSelectedLevel = i;
				//    //
				//    //EvtSelectEntity ev{};
				//    //ev.Entity = static_pointer_cast<CEntity>(pair.second);
				//    //CGameInstance::GetInstance()->Get_EventBus()->Publish(ev);


				//}
				// 2. [핵심] 클릭 시 현재 태그와 레벨을 멤버 변수에 저장
				if (ImGui::Selectable(tagStr.c_str(), isSelected))
				{
					m_strSelectedTag = pair.first;   // wstring 저장
					m_iSelectedLevel = i;            // 현재 레벨 인덱스 저장
				}
				// 드래그 소스 설정
				if (ImGui::BeginDragDropSource()) {
					// 구조체 전체를 메모리 덩어리로 보냄
					DragDropProto protoClone_Desc = {};
					//wcscpy_s(payload.szTag, pair.first.c_str());
					wcscpy_s(protoClone_Desc.szTag, pair.first.c_str());
					protoClone_Desc.iLevel = i;
					ImGui::SetDragDropPayload("COMP_DRAG_DATA", &protoClone_Desc, sizeof(DragDropProto));

					ImGui::Text("Swap to %s", Engine::W2S(protoClone_Desc.szTag).c_str());
					ImGui::EndDragDropSource();
				}
			}

			// [중요] TreeNode가 열렸을 때만 Pop을 한 번 해줘야 합니다.
			ImGui::TreePop();
		}
	}
	ImGui::Separator();

	ImGui::Separator();

	// 2. 추가 버튼 로직 (즉시 생성 및 레이어 추가)
	if (m_strSelectedTag != L"")
	{
		string btnLabel = "Add [" + Engine::W2S(m_strSelectedTag) + "]";

		// 버튼 클릭 시 즉시 실행
		if (ImGui::Button(btnLabel.c_str(), ImVec2(-1, 30)))
		{


			// [2] 현재 활성화된 레벨의 특정 레이어에 즉시 추가
			// 레이어 태그는 프로젝트 환경에 맞게 수정하세요 (예: TEXT("Layer_Object"))
			_uint iCurrentLevel = m_pGameInstance.lock()->Get_Current_LevelIdx();
			if (nullptr==(m_pGameInstance.lock()->Add_GameObject(m_iSelectedLevel,m_strSelectedTag, iCurrentLevel,L"Static")))
			{
				MSG_BOX("Failed to Add GameObject to Layer");
			}



		}
	}
	else
	{
		ImGui::TextDisabled("Select a prototype to add.");
	}

}


shared_ptr<CBase> CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevel)
		return nullptr;

	auto iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

unique_ptr<CPrototype_Manager> CPrototype_Manager::Create(_uint iNumLevel)
{
	unique_ptr<CPrototype_Manager> pInstance(new CPrototype_Manager());

	if (FAILED(pInstance->Initialize(iNumLevel)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
	}
	return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumLevel;i++)
	{
		//for(auto& pair : m_pPrototypes[i])
		//{
	 //       Safe_Release(pair.second);
		//}
		m_pPrototypes[i].clear();
	}
	Safe_Delete_Array(m_pPrototypes); // 배열지우는 매크로 
}
