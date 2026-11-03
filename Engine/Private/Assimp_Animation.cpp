#include "Assimp_Animation.h"
#include "Assimp_Channel.h"
#include "Assimp_Bone.h"
#include "Assimp_Model.h"

CAssimp_Animation::CAssimp_Animation()
{
}

HRESULT CAssimp_Animation::Initialize(const aiAnimation* srcAIAnimation, shared_ptr<class CAssimp_Model> model)
{
    m_duration = srcAIAnimation->mDuration;
    m_tickPerSecond = srcAIAnimation->mTicksPerSecond;

    m_numChannels = srcAIAnimation->mNumChannels; // 애니메이션을 재생하는데 필요한 뼈의 갯수

    for(size_t i =0;i< m_numChannels;i++)
    {
        auto channel = CAssimp_Channel::Create(srcAIAnimation->mChannels[i], model);
        if (nullptr == channel)
            return E_FAIL;

        m_channels.push_back(channel);
    }
    return S_OK;
}

_bool CAssimp_Animation::Update_TransformationMatrices(_float timeDelta, const vector<shared_ptr<CAssimp_Bone>>& Bones, _bool isLoop)
{
    m_currentTrackPosition += m_tickPerSecond * timeDelta;

    if(m_currentTrackPosition >= m_duration)
    {
        if (false == isLoop)
            return true;

        m_currentTrackPosition = 0.f;
    }

    for(auto& channel:m_channels)
    {
        channel->Update_TransformationMatrix(m_currentTrackPosition, Bones);
    }
    return false;
}

shared_ptr<CAssimp_Animation> CAssimp_Animation::Create(const aiAnimation* srcAIAnimation, shared_ptr<class CAssimp_Model> model)
{
    shared_ptr<CAssimp_Animation> instance(new CAssimp_Animation(), [](CAssimp_Animation* p) {p->Free(); delete p; });

    if (FAILED(instance->Initialize(srcAIAnimation, model)))
    {
        MSG_BOX("Failed to Created : CAssimp_Animation");
    }
    return instance;
}

void CAssimp_Animation::Free()
{
	__super::Free();

    m_channels.clear();
}
