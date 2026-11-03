#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_gameInstance(CGameInstance::GetInstance())
{

}

HRESULT CLevel_Manager::Initialize(_uint EditorIndex, _uint iNumLevel)
{
	m_editorIndex = EditorIndex;

	m_numLevel = iNumLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Change_Level(_uint iNewLevelIndex, shared_ptr<CLevel> pNewLevel)
{
	if (m_curLevel != nullptr)
	{


		if (m_curLevelIdx == m_editorIndex && m_editorIndex!=0)
		{
			for (_uint i = 1; i < m_numLevel;i++)
			{
				//스태틱레벨은 0 번 레벨 고정이므로 1부터 시작
				m_gameInstance.lock()->Clear_Resources(i);

			}
		}
		else
		{
			m_gameInstance.lock()->Clear_Resources(m_curLevelIdx);
		}
	}


	m_curLevel = pNewLevel;

	m_curLevelIdx = iNewLevelIndex;

	return m_curLevel->Post_Initialize();

}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (m_curLevel != nullptr)
		m_curLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (m_curLevel != nullptr)
		m_curLevel->Render();

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
