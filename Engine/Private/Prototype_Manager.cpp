#include "Prototype_Manager.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevel = iNumLevels;

    m_pPrototypes = new PROTOTYPES[iNumLevels];

    return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
    if (iLevelIndex >= m_iNumLevel) // 배열 인댁스라서 
        return E_FAIL;

    // 같은 키를 가진 객체가 있는지 확인
    if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
    {
        MSG_BOX("An object with the same key already exists.");
        return E_FAIL;
    }

    m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

    return S_OK;
}

HRESULT CPrototype_Manager::Clear_Prototype(_uint iLevelIndex)
{
    if (iLevelIndex >= m_iNumLevel)
        return E_FAIL;

    for (auto& pair : m_pPrototypes[iLevelIndex])
    {
        Safe_Release(pair.second);
    }
    m_pPrototypes[iLevelIndex].clear();

    return S_OK;

}


CBase* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
    if (iLevelIndex >= m_iNumLevel)
        return nullptr;

    auto iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
    if (iter == m_pPrototypes[iLevelIndex].end())
        return nullptr;

    return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevel)
{
    CPrototype_Manager* pInstance = new CPrototype_Manager();

    if (FAILED(pInstance->Initialize(iNumLevel)))
    {
        MSG_BOX("Failed to Created : CPrototype_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

    for(int i =0; i < m_iNumLevel;i++)
    {
	    for(auto& pair : m_pPrototypes[i])
	    {
            Safe_Release(pair.second);
	    }
        m_pPrototypes[i].clear();
    }
    Safe_Delete_Array(m_pPrototypes); // 배열지우는 매크로 
}
