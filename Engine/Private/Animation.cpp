#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype) :
	m_duration{ Prototype.m_duration },
	m_tickPerSecond{ Prototype.m_tickPerSecond },
	m_currentTrackPosition{ Prototype.m_currentTrackPosition },
	m_numChannels{ Prototype.m_numChannels },
	m_channels{ Prototype.m_channels },
	m_currentKeyFrameIndices{ Prototype.m_currentKeyFrameIndices }
{
}

HRESULT CAnimation::Initialize(ifstream& InFile)
{
	Cvt_Animation animationDesc = {};
	InFile.read((char*)&animationDesc, sizeof(Cvt_Animation));


	m_duration = animationDesc.duration;
	m_tickPerSecond = animationDesc.tickPerSecond;

	m_numChannels = animationDesc.numChannels; // 애니메이션을 재생하는데 필요한 뼈의 갯수

	m_currentKeyFrameIndices.resize(m_numChannels);

	for (size_t i = 0; i < m_numChannels; i++)
	{
		auto channel = CChannel::Create(InFile);
		if (nullptr == channel)
			return E_FAIL;

		m_channels.push_back(channel);
	}
	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(_float timeDelta, const vector<shared_ptr<CBone>>& Bones, _bool isLoop)
{
	m_currentTrackPosition += m_tickPerSecond * timeDelta;

	if (m_currentTrackPosition >= m_duration)
	{
		if (false == isLoop)
		{
			for (size_t i = 0; i < m_numChannels; i++)
			{
				m_channels[i]->Update_TransformationMatrix(&m_currentKeyFrameIndices[i], m_currentTrackPosition, Bones);
			}
			return true;
		}

		m_currentTrackPosition = 0.f;
	}

	//for(auto& pChannel:m_Channels)

	for (size_t i = 0; i < m_numChannels; i++)
	{
		m_channels[i]->Update_TransformationMatrix(&m_currentKeyFrameIndices[i], m_currentTrackPosition, Bones);
	}
	return false;
}

_bool CAnimation::Blend_TransformationMatrices(_float timeDelta, const shared_ptr<CAnimation>& nextAnim,
	_float blendRatio, const vector<shared_ptr<CBone>>& bones, _bool isCurLoop, _bool isNextLoop)
{
	vector<_matrix> poseA;
	vector<_matrix> poseB;
	// A
	auto backupA = m_currentKeyFrameIndices;
	float backupTimeA = m_currentTrackPosition;

	Update_ToBuffer(timeDelta, poseA, isCurLoop, bones.size());

	m_currentKeyFrameIndices = backupA;
	m_currentTrackPosition = backupTimeA;

	// B 
	auto backupB = nextAnim->m_currentKeyFrameIndices;
	float backupTimeB = nextAnim->m_currentTrackPosition;

	nextAnim->Update_ToBuffer(timeDelta, poseB, isNextLoop, bones.size());

	nextAnim->m_currentKeyFrameIndices = backupB;
	nextAnim->m_currentTrackPosition = backupTimeB;
	for (int boneIndex = 0; boneIndex < bones.size(); ++boneIndex)
	{
		_vector sA, rA, tA;
		_vector sB, rB, tB;

		bool validA = XMMatrixDecompose(&sA, &rA, &tA, poseA[boneIndex]);
		bool validB = XMMatrixDecompose(&sB, &rB, &tB, poseB[boneIndex]);

		if (!validA && !validB)
			continue;

		if (!validA)
		{
			bones[boneIndex]->Update_TransformationMatrix(poseB[boneIndex]);
			continue;
		}

		if (!validB)
		{
			bones[boneIndex]->Update_TransformationMatrix(poseA[boneIndex]);
			continue;
		}

		_vector scale = XMVectorLerp(sA, sB, blendRatio);
		_vector rot = XMQuaternionNormalize(XMQuaternionSlerp(rA, rB, blendRatio));
		_vector pos = XMVectorLerp(tA, tB, blendRatio);

		_matrix mat = XMMatrixAffineTransformation(scale, XMVectorZero(), rot, pos);

		bones[boneIndex]->Update_TransformationMatrix(mat);
	}
	return true;
}
void CAnimation::Update_ToBuffer(
	_float timeDelta,
	vector<_matrix>& outMatrices,
	_bool isLoop ,_uint boneCount)
{
	
	outMatrices.resize(boneCount);

	for (auto& mat : outMatrices)
		mat = XMMatrixIdentity();

	// 시간 업데이트
	m_currentTrackPosition += m_tickPerSecond * timeDelta;

	if (m_currentTrackPosition >= m_duration)
	{
		if (!isLoop)
		{
			for (size_t i = 0; i < m_numChannels; i++)
			{
				m_channels[i]->Update_ToMatrix(
					&m_currentKeyFrameIndices[i],
					m_currentTrackPosition,
					outMatrices);
			}
			return;
		}

		m_currentTrackPosition = 0.f;
	}

	for (size_t i = 0; i < m_numChannels; i++)
	{
		m_channels[i]->Update_ToMatrix(
			&m_currentKeyFrameIndices[i],
			m_currentTrackPosition,
			outMatrices);
	}
}
shared_ptr<CAnimation> CAnimation::Create(ifstream& InFile)
{
	shared_ptr<CAnimation> instance(new CAnimation(), [](CAnimation* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(InFile)))
	{
		MSG_BOX("Failed to Created : CAnimation");
	}
	return instance;
}

shared_ptr<CAnimation> CAnimation::Clone()
{
	shared_ptr<CAnimation> instance(new CAnimation(*this), [](CAnimation* p) {p->Free(); delete p; });
	return instance;
}

void CAnimation::Free()
{
	__super::Free();

	m_channels.clear();
}
