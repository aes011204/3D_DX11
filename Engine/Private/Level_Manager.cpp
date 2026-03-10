#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{

}

HRESULT CLevel_Manager::Initialize(_uint EditorIndex, _uint iNumLevel)
{
	m_iEditorIndex = EditorIndex;

	m_iNumLevel = iNumLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Change_Level(_uint iNewLevelIndex, shared_ptr<CLevel> pNewLevel)
{
	if (m_pCurLevel != nullptr)
	{


		if (m_iCurLevelIdx == m_iEditorIndex && m_iEditorIndex!=0)
		{
			for (_uint i = 1; i < m_iNumLevel;i++)
			{
				//스태틱레벨은 0 번 레벨 고정이므로 1부터 시작
				m_pGameInstance.lock()->Clear_Resources(i);

			}
		}
		else
		{
			m_pGameInstance.lock()->Clear_Resources(m_iCurLevelIdx);
		}
	}


	m_pCurLevel = pNewLevel;

	m_iCurLevelIdx = iNewLevelIndex;

	return m_pCurLevel->Post_Initialize();

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

unique_ptr<CLevel_Manager> CLevel_Manager::Create(_uint EditorIndex, _uint iNumLevel)
{
	unique_ptr<CLevel_Manager> pInstance(new CLevel_Manager());
	if (FAILED(pInstance->Initialize(EditorIndex, iNumLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
	}
	return pInstance;
}

void CLevel_Manager::Free()
{
	__super::Free();

}
