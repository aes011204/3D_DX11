#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype) :
	m_fDuration{ Prototype.m_fDuration },
	m_fTickPerSecond{ Prototype.m_fTickPerSecond },
	m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition },
	m_iNumChannels{ Prototype.m_iNumChannels },
	m_Channels{ Prototype.m_Channels },
	m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
{
}

HRESULT CAnimation::Initialize(ifstream& InFile)
{
	Cvt_Animation animationDesc = {};
	InFile.read((char*)&animationDesc, sizeof(Cvt_Animation));


	m_fDuration = animationDesc.dDuration;
	m_fTickPerSecond = animationDesc.dTickPerSecond;

	m_iNumChannels = animationDesc.iNumChannels; // 애니메이션을 재생하는데 필요한 뼈의 갯수

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
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

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (false == isLoop)
		{
			for (size_t i = 0; i < m_iNumChannels; i++)
			{
				m_Channels[i]->Update_TransformationMatrix(&m_CurrentKeyFrameIndices[i], m_fCurrentTrackPosition, Bones);
			}
			return true;
		}

		m_fCurrentTrackPosition = 0.f;
	}

	//for(auto& pChannel:m_Channels)

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update_TransformationMatrix(&m_CurrentKeyFrameIndices[i], m_fCurrentTrackPosition, Bones);
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

shared_ptr<CAnimation> CAnimation::Clone()
{
	shared_ptr<CAnimation> pInstance(new CAnimation(*this), [](CAnimation* p) {p->Free(); delete p; });
	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	m_Channels.clear();
}
