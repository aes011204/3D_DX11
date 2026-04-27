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
_uint CMiniGame::SelectMutationRandom(const Fish_Def& fishDef)
{
    vector<ID_uint> candidates;
    candidates.push_back(ID_Absence);

    for (const Mutation& mut : fishDef.vec_Mutation)
        candidates.push_back(mut.Mutation_ID);

    int index = m_pGameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
    return candidates[index];
}
_uint CMiniGame::SelectMutationNoRepeat(const Fish_Def& fishDef)
{
    vector<ID_uint> candidates;
    candidates.push_back(ID_Absence); // 기본형 포함

    for (const Mutation& mut : fishDef.vec_Mutation)
        candidates.push_back(mut.Mutation_ID);

    if (candidates.empty())
        return ID_Absence;

    if (candidates.size() == 1)
        return candidates[0];

    int index = m_pGameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
    ID_uint pickedID = candidates[index];

    while (pickedID == m_PrevFishID)
    {
        index = m_pGameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
        pickedID = candidates[index];
    }

    m_PrevFishID = pickedID;
    return pickedID;
}

void CMiniGame::Free()
{
	CBase::Free();
}
