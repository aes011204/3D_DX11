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

template <typename T>
HRESULT CEntity::Add_Component(_uint iProtoLevelIndex, const _wstring& strProtoTag, const _wstring& strComponentTag,
	T** ppOut, void* pArg)
{

		shared_ptr<CComponent> tmpComp = Get_Component(strComponentTag);
		if (tmpComp != nullptr)
		{
			MSG_BOX("Failed Add Component : Same Key exist");
			return E_FAIL;
		}

		shared_ptr<CComponent> CloneComp = dynamic_cast<shared_ptr<CComponent>>(
			m_pGameInstance.lock()->Clone_Prototype(PROTOTYPE::COMPONENT, iProtoLevelIndex, strProtoTag, pArg));
		if (CloneComp == nullptr)
			return E_FAIL;

		T* pCastComp = dynamic_cast<T*>(CloneComp);
		if (pCastComp == nullptr)
		{
			MSG_BOX("Failed Add Component : Wrong Type(it's should Component's Child)");
			return E_FAIL;
		}
		*ppOut = pCastComp;

		m_Components.emplace(strComponentTag, pCastComp);

		return S_OK;
	
}

void CEntity::Free()
{
	__super::Free();

	//for (auto& Pair : m_Components)
	//	Safe_Release(Pair.second);
	m_Components.clear();

}
