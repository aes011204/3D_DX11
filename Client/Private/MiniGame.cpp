#include "MiniGame.h"
#include "GameInstance.h"

CMiniGame::CMiniGame() : m_gameInstance(CGameInstance::GetInstance())
{
}

HRESULT CMiniGame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame::Initialize(void* pArg)
{

	MiniGameDesc* pDesc = static_cast<MiniGameDesc*>(pArg);

	m_defId = pDesc->DefID;
	m_rodSpeed = pDesc->RodSpeed;
	m_fishCount = pDesc->FishCount;
	m_miniGameType = pDesc->MiniGameType;
	m_invenCtrl = pDesc->pInvenCtrl;

	return S_OK;
}

HRESULT CMiniGame::Render(_uint iMeshIndex)
{
	return S_OK;

}

void CMiniGame::Update(const _float& timeDelta)
{
}
_uint CMiniGame::SelectMutationRandom(const FishDef& fishDef)
{
    vector<IdType> candidates;
    candidates.push_back(kIdAbsence);

    for (const Mutation& mut : fishDef.vecMutation)
        candidates.push_back(mut.mutationId);

    int index = m_gameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
    return candidates[index];
}
_uint CMiniGame::SelectMutationNoRepeat(const FishDef& fishDef)
{
    vector<IdType> candidates;
    candidates.push_back(kIdAbsence); // 기본형 포함

    for (const Mutation& mut : fishDef.vecMutation)
        candidates.push_back(mut.mutationId);

    if (candidates.empty())
        return kIdAbsence;

    if (candidates.size() == 1)
        return candidates[0];

    int index = m_gameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
    IdType pickedID = candidates[index];

    while (pickedID == m_prevFishId)
    {
        index = m_gameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
        pickedID = candidates[index];
    }

    m_prevFishId = pickedID;
    return pickedID;
}

void CMiniGame::Free()
{
	CBase::Free();
}
