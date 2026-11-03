#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_gameInstance(CGameInstance::GetInstance())
{

}

HRESULT CLevel_Manager::Initialize(_uint editorIndex, _uint numLevel)
{
	m_editorIndex = editorIndex;

	m_numLevel = numLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Change_Level(_uint newLevelIndex, shared_ptr<CLevel> newLevel)
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


	m_curLevel = newLevel;

	m_curLevelIdx = newLevelIndex;

	return m_curLevel->Post_Initialize();

}

void CLevel_Manager::Update(_float timeDelta)
{
	if (m_curLevel != nullptr)
		m_curLevel->Update(timeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (m_curLevel != nullptr)
		m_curLevel->Render();

	return S_OK;
}

unique_ptr<CLevel_Manager> CLevel_Manager::Create(_uint editorIndex, _uint numLevel)
{
	unique_ptr<CLevel_Manager> instance(new CLevel_Manager());
	if (FAILED(instance->Initialize(editorIndex, numLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
	}
	return instance;
}

void CLevel_Manager::Free()
{
	__super::Free();

}
