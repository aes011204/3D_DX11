#include "Assimp_Animation.h"
#include "Assimp_Channel.h"
#include "Assimp_Bone.h"
#include "Assimp_Model.h"

CAssimp_Animation::CAssimp_Animation()
{
}

HRESULT CAssimp_Animation::Initialize(const aiAnimation* pAIAnimation, shared_ptr<class CAssimp_Model> pModel)
{
    m_fDuration = pAIAnimation->mDuration;
    m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

    m_iNumChannels = pAIAnimation->mNumChannels; // 애니메이션을 재생하는데 필요한 뼈의 갯수

    for(size_t i =0;i< m_iNumChannels;i++)
    {
        auto pChannel = CAssimp_Channel::Create(pAIAnimation->mChannels[i], pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }
    return S_OK;
}

_bool CAssimp_Animation::Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<CAssimp_Bone>>& Bones, _bool isLoop)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if(m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
            return true;

        m_fCurrentTrackPosition = 0.f;
    }

    for(auto& pChannel:m_Channels)
    {
        pChannel->Update_TransformationMatrix(m_fCurrentTrackPosition, Bones);
    }
    return false;
}

shared_ptr<CAssimp_Animation> CAssimp_Animation::Create(const aiAnimation* pAIAnimation, shared_ptr<class CAssimp_Model> pModel)
{
    shared_ptr<CAssimp_Animation> pInstance(new CAssimp_Animation(), [](CAssimp_Animation* p) {p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
    {
        MSG_BOX("Failed to Created : CAssimp_Animation");
    }
    return pInstance;
}

void CAssimp_Animation::Free()
{
	__super::Free();

    m_Channels.clear();
}
