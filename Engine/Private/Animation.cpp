#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(ifstream& InFile)
{
    Cvt_Animation animationDesc = {};
    InFile.read((char*)&animationDesc, sizeof(Cvt_Animation));


    m_fDuration = animationDesc.dDuration;
    m_fTickPerSecond = animationDesc.dTickPerSecond;

    m_iNumChannels = animationDesc.iNumChannels; // 애니메이션을 재생하는데 필요한 뼈의 갯수

    for(size_t i =0;i< m_iNumChannels;i++)
    {
        auto pChannel = CChannel::Create(InFile);
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

shared_ptr<CAnimation> CAnimation::Create(ifstream& InFile)
{
    shared_ptr<CAnimation> pInstance(new CAnimation(), [](CAnimation* p) {p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(InFile)))
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
