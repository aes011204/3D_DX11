#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{

}

HRESULT CLevel_Manager::Change_Level(_uint iNewLevelIndex, shared_ptr<CLevel> pNewLevel)
{
	if (m_pCurLevel != nullptr)
		m_pGameInstance.lock()->Clear_Resources(m_iCurLevelIdx);

	//if (0 != Safe_Release(m_pCurLevel))
	//{
	//	return E_FAIL;
	//}

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

unique_ptr<CLevel_Manager> CLevel_Manager::Create()
{
	unique_ptr<CLevel_Manager> pInstance(new CLevel_Manager());
	return pInstance; // 이니셜라이즈 필요 없음
}

void CLevel_Manager::Free()
{
	__super::Free();

}
