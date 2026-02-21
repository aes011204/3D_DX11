#include "Entity.h"
#include "Component.h"
#include "GameInstance.h"

CEntity::CEntity(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{

}

CEntity::CEntity(const CEntity& prototype)
	: m_pDevice{ prototype.m_pDevice }, m_pContext{ prototype.m_pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
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
		return S_OK;
	}

	m_bIsDirtyCom = true;

	return E_FAIL;
}

void CEntity::Load_FromJson(nlohmann::json& j)
{



}



void CEntity::Free()
{
	__super::Free();

	//for (auto& Pair : m_Components)
	//	Safe_Release(Pair.second);
	m_Components.clear();

	//m_pDevice.Reset();
	//m_pContext.Reset();
}
