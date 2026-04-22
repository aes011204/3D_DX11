#include "MiniGame.h"
#include "GameInstance.h"

CMiniGame::CMiniGame() : m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CMiniGame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame::Initialize(void* pArg)
{

	MINIGAEME_L_DESC* pDesc = static_cast<MINIGAEME_L_DESC*>(pArg);

	m_DefID = pDesc->DefID;
	m_RodSpeed = pDesc->RodSpeed;
	m_FishCount = pDesc->FishCount;
	m_MiniGameType = pDesc->MiniGameType;
	m_InvenCtrl = pDesc->pInvenCtrl;

	return S_OK;
}

HRESULT CMiniGame::Render(_uint iMeshIndex)
{
	return S_OK;

}

void CMiniGame::Update(const _float& timeDelta)
{
}



void CMiniGame::Free()
{
	CBase::Free();
}
