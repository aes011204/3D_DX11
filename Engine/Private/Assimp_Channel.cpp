#include "Assimp_Channel.h"

#include "Assimp_Model.h"
#include "Assimp_Bone.h"

CAssimp_Channel::CAssimp_Channel()
{
}


HRESULT CAssimp_Channel::Initialize(const aiNodeAnim* srcAIChannel, shared_ptr<class CAssimp_Model> model)
{
    m_boneIndex = model->Get_BoneIndex(srcAIChannel->mNodeName.data);

	m_numKeyFrames = max(srcAIChannel->mNumPositionKeys, srcAIChannel->mNumRotationKeys);
	m_numKeyFrames = max(m_numKeyFrames, srcAIChannel->mNumScalingKeys);

    _float3 scale = {};
    _float4 rotation = {};
    _float3 translation = {};

    for(size_t i =0;i<m_numKeyFrames;i++)
    {
        KeyFrame keyFrame = {};
        if(srcAIChannel->mNumScalingKeys > i)
        {
            memcpy(&scale, &srcAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            keyFrame.trackPosition = srcAIChannel->mScalingKeys[i].mTime; // 이거 실제 시간이 아님
        }
        if (srcAIChannel->mNumRotationKeys > i)
        {
            rotation.x = srcAIChannel->mRotationKeys[i].mValue.x;
            rotation.y = srcAIChannel->mRotationKeys[i].mValue.y;
            rotation.z = srcAIChannel->mRotationKeys[i].mValue.z;
            rotation.w = srcAIChannel->mRotationKeys[i].mValue.w;

            keyFrame.trackPosition = srcAIChannel->mRotationKeys[i].mTime;
        }
        if (srcAIChannel->mNumPositionKeys > i)
        {
            memcpy(&translation, &srcAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            keyFrame.trackPosition = srcAIChannel->mPositionKeys[i].mTime; // 이거 실제 시간이 아님
        }

        keyFrame.scale = scale;
        keyFrame.rotation = rotation;
        keyFrame.translation = translation;

        m_keyFrames.push_back(keyFrame);
    }

     
    return S_OK;
}

void CAssimp_Channel::Update_TransformationMatrix(_float currentTrackPosition, const vector<shared_ptr<CAssimp_Bone>>& Bones)
{
    if (0.f == currentTrackPosition)
        m_currentKeyFrameIndex = 0;

    KeyFrame lastKeyFrame = m_keyFrames.back();

    _vector scale, rotation, translation;

    if (currentTrackPosition >= lastKeyFrame.trackPosition)// 마지막 프레임 이후 그 자세 그대로 유지
    {
        scale = XMLoadFloat3(&lastKeyFrame.scale);
        rotation = XMLoadFloat4(&lastKeyFrame.rotation);
        translation = XMVectorSetW(XMLoadFloat3(&lastKeyFrame.translation), 1.f);
    }
    else // 사이보간
    {
        while (currentTrackPosition >= m_keyFrames[m_currentKeyFrameIndex + 1].trackPosition)
            ++m_currentKeyFrameIndex;

        _float ratio = (currentTrackPosition - m_keyFrames[m_currentKeyFrameIndex].trackPosition) /
            (m_keyFrames[m_currentKeyFrameIndex + 1].trackPosition - m_keyFrames[m_currentKeyFrameIndex].trackPosition);

        _vector leftScale, rightScale;
        _vector	leftRotation, rightRotation;
        _vector	leftTranslation, rightTranslation;

        leftScale = XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex].scale);
        rightScale = XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex+1].scale);
        scale = XMVectorLerp(leftScale, rightScale, ratio);

        leftRotation = XMLoadFloat4(&m_keyFrames[m_currentKeyFrameIndex].rotation);
        rightRotation = XMLoadFloat4(&m_keyFrames[m_currentKeyFrameIndex + 1].rotation);
        rotation = XMQuaternionSlerp(leftRotation, rightRotation, ratio);

        leftTranslation = XMVectorSetW(XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex].translation), 1.f);
        rightTranslation = XMVectorSetW(XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex + 1].translation), 1.f);
        translation = XMVectorLerp(leftTranslation, rightTranslation, ratio);
    }

    _matrix		boneTransformationMatrix = XMMatrixAffineTransformation(
        scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), rotation, translation);

    Bones[m_boneIndex]->Update_TransformationMatrix(boneTransformationMatrix);
}

shared_ptr<CAssimp_Channel> CAssimp_Channel::Create(const aiNodeAnim* srcAIChannel, shared_ptr<class CAssimp_Model> model)
{
    shared_ptr<CAssimp_Channel> instance(new CAssimp_Channel(), [](CAssimp_Channel* p) {p->Free(); delete p; });

    if (FAILED(instance->Initialize(srcAIChannel, model)))
    {
        MSG_BOX("Failed to Created : CAssimp_Channel");
    }
    return instance;
}


void CAssimp_Channel::Free()
{
	__super::Free();
    m_keyFrames.clear();
}
