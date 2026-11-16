#include "MiniGame.h"
#include "GameInstance.h"

CMiniGame::CMiniGame() : m_gameInstance(CGameInstance::GetInstance())
{
}

HRESULT CMiniGame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame::Initialize(void* arg)
{

	MiniGameDesc* desc = static_cast<MiniGameDesc*>(arg);

	m_defID = desc->defID;
	m_rodSpeed = desc->rodSpeed;
	m_fishCount = desc->fishCount;
	m_miniGameType = desc->miniGameType;
	m_invenCtrl = desc->invenCtrl;

	return S_OK;
}

HRESULT CMiniGame::Render(_uint meshIndex)
{
	return S_OK;

}

void CMiniGame::Update(const _float& timeDelta)
{
}
_uint CMiniGame::SelectMutationRandom(const FishDef& fishDef)
{
    vector<IDType> candidates;
    candidates.push_back(kIDAbsence);

    for (const Mutation& mut : fishDef.vecMutation)
        candidates.push_back(mut.mutationID);

    int index = m_gameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
    return candidates[index];
}
_uint CMiniGame::SelectMutationNoRepeat(const FishDef& fishDef)
{
    vector<IDType> candidates;
    candidates.push_back(kIDAbsence); // 기본형 포함

    for (const Mutation& mut : fishDef.vecMutation)
        candidates.push_back(mut.mutationID);

    if (candidates.empty())
        return kIDAbsence;

    if (candidates.size() == 1)
        return candidates[0];

    int index = m_gameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
    IDType pickedID = candidates[index];

    while (pickedID == m_prevFishID)
    {
        index = m_gameInstance.lock()->RandomInt(0, static_cast<int>(candidates.size()) - 1);
        pickedID = candidates[index];
    }

    m_prevFishID = pickedID;
    return pickedID;
}

void CMiniGame::Free()
{
	CBase::Free();
}
