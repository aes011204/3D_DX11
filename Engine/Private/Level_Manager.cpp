#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pCurLevel);
}

HRESULT CLevel_Manager::Change_Level(_uint iNewLevelIndex, CLevel* pNewLevel)
{
	if (m_pCurLevel != nullptr)
		m_pGameInstance->Clear_Resources(iNewLevelIndex);

	if(0 != Safe_Release(m_pCurLevel))
	{
		return E_FAIL;
	}

	m_pCurLevel = pNewLevel;

	m_iCurLevelIdx = iNewLevelIndex;

	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (m_pCurLevel != nullptr)
		m_pCurLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (m_pCurLevel != nullptr)
		m_pCurLevel->Render();

	return S_OK;
}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager(); // 이니셜라이즈 필요 없음
}

void CLevel_Manager::Free()
{
	CBase::Free();
	Safe_Release(m_pCurLevel);


}
