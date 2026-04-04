#include "Entity.h"
#include "Component.h"
#include "GameInstance.h"
#include "Engine_Helper.h"

CEntity::CEntity(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{

}

CEntity::CEntity(const CEntity& prototype)
	: m_pDevice{ prototype.m_pDevice }, m_pContext{ prototype.m_pContext },
	m_pGameInstance{ CGameInstance::GetInstance() },
	m_bIsSavableClass{ m_bIsSavableClass }
{

	//  레퍼런스 관리를 위해 디폴트 복사생성자가 아니라 만드거임
}

shared_ptr<CComponent> CEntity::Get_Component(const _wstring& strComponentTag)
{
	auto it = m_Components.find(strComponentTag);
	if (it == m_Components.end())
		return nullptr;

	return it->second;
}

//void CEntity::Change_Component(const _wstring& strComponentTag)
//{
//	// 지우고 
//	Remove_Component(strComponentTag);
//		//같은 태그에  추가하기
//	Add_Component();;

//}

HRESULT CEntity::Remove_Component(const _wstring& strComponentTag)
{
	auto com = m_Components.find(strComponentTag);
	if (com != m_Components.end()) // 찾았을 때만 지우기
	{
		m_Components.erase(com);
	m_bIsDirtyCom = true;
		return S_OK;
	}


	return E_FAIL;
}

void CEntity::Load_FromJson(nlohmann::json& j)
{
	shared_ptr<CComponent> nothing = {};
	for (auto& jCom : j["Components"])
	{
		if (jCom["ComponentTag"] == "Com_Transform")
		{
			//Add_Component<CComponent>((_uint)jCom["ComProtoLevel"], S2W(jCom["ComProtoTag"]), S2W(jCom["ComponentTag"]), nullptr, nullptr);

			m_Components.find(S2W(jCom["ComponentTag"]))->second->Load_FromJson(jCom);

		}
		else if(jCom["ComponentTag"] == "Com_UITransform")
		{
			m_Components.find(S2W(jCom["ComponentTag"]))->second->Load_FromJson(jCom);

		}
		else if (jCom["ComponentTag"] != "Com_UITransform" && jCom["ComponentTag"] != "Com_Transform")
		{
			if (FAILED(Add_Component<CComponent>((_uint)jCom["ComProtoLevel"], S2W(jCom["ComProtoTag"]), S2W(jCom["ComponentTag"]), nullptr, nullptr)))
			{
				MSG_BOX("fail to add Component while data load");
			}

			auto it = m_Components.find(S2W(jCom["ComponentTag"]));

			if (it != m_Components.end() && it->second != nullptr)
			{
				it->second->Load_FromJson(jCom);
			}
			else
			{
				// 디버깅용: 어떤 태그를 못 찾았는지 확인
				string tag = jCom.value("ComponentTag", "Unknown");
				string str = "Component Tag Not Found: " + tag;
				LOG(LOG_LEVEL::WARNING, str.c_str());
			}


		}
	}

}



void CEntity::Free()
{
	__super::Free();

	m_Components.clear();

}
