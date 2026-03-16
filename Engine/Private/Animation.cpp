#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, CModel* pModel)
{
    m_fDuration = pAIAnimation->mDuration;
    m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

    m_iNumChannels = pAIAnimation->mNumChannels; // 애니메이션을 재생하는데 필요한 뼈의 갯수

    for(size_t i =0;i< m_iNumChannels;i++)
    {
        auto pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }
    return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(_float fTimeDelta, const vector<shared_ptr<CBone>>& Bones, _bool isLoop)
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

shared_ptr<CAnimation> CAnimation::Create(const aiAnimation* pAIAnimation, CModel* pModel)
{
    shared_ptr<CAnimation> pInstance(new CAnimation(), [](CAnimation* p) {p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
    {
        MSG_BOX("Failed to Created : CAnimation");
    }
    return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

    m_Channels.clear();
}
