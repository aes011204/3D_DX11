#include "Channel.h"

#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}


HRESULT CChannel::Initialize(ifstream& InFile)
{

    Cvt_Channel channelDesc = {};
    InFile.read((char*)&channelDesc, sizeof(Cvt_Channel));




    m_boneIndex = channelDesc.boneIndex;

    m_numKeyFrames = channelDesc.numKeyframes;

    m_keyFrames.reserve(m_numKeyFrames); // 미리 공간 확보

    for (size_t i = 0; i < m_numKeyFrames; i++)
    {
        Cvt_Keyframe keyFrameDesc = {};
        InFile.read((char*)&keyFrameDesc, sizeof(Cvt_Keyframe));

        KeyFrame keyFrame = {};
        // 실제 컨버터에서 저장한 시간을 트랙 포지션으로 사용
        keyFrame.trackPosition = (float)keyFrameDesc.trackPosition;

        // 데이터 복사 (Cvt_Keyframe -> KEYFRAME)
        memcpy(&keyFrame.scale, keyFrameDesc.scale, sizeof(_float3));
        memcpy(&keyFrame.rotation, keyFrameDesc.rotation, sizeof(_float4)); // qRot 사용
        memcpy(&keyFrame.translation, keyFrameDesc.pos, sizeof(_float3)); // vPos 사용

        m_keyFrames.push_back(keyFrame);
    }




//    _float3 vScale = {};
//    _float4 vRotation = {};
//    _float3 vTranslation = {};
//
//    for(size_t i =0;i<m_iNumKeyFrames;i++)
//    {
//        KEYFRAME KeyFrame = {};
//        if(pAIChannel->mNumScalingKeys > i)
//        {
//            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
//            KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime; // 이거 실제 시간이 아님
//        }
//        if (pAIChannel->mNumRotationKeys > i)
//        {
//            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
//            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
//            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
//            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
//
//            KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
//        }
//        if (pAIChannel->mNumPositionKeys > i)
//        {
//            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
//            KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime; // 이거 실제 시간이 아님
//        }
//
//        KeyFrame.vScale = vScale;
//        KeyFrame.vRotation = vRotation;
//        KeyFrame.vTranslation = vTranslation;
//
//        m_KeyFrames.push_back(KeyFrame);
//    }
//
//     
    return S_OK;
}

void CChannel::Update_TransformationMatrix(_uint* currentKeyFrameIndex, _float currentTrackPosition, const vector<shared_ptr<CBone>>& Bones)
{
    if (0.f == currentTrackPosition)
        (*currentKeyFrameIndex) = 0;

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
        while (currentTrackPosition >= m_keyFrames[(*currentKeyFrameIndex) + 1].trackPosition)
            ++(*currentKeyFrameIndex);

        _float ratio = (currentTrackPosition - m_keyFrames[(*currentKeyFrameIndex)].trackPosition) /
            (m_keyFrames[(*currentKeyFrameIndex) + 1].trackPosition - m_keyFrames[(*currentKeyFrameIndex)].trackPosition);

        _vector leftScale, rightScale;
        _vector	leftRotation, rightRotation;
        _vector	leftTranslation, rightTranslation;

        leftScale = XMLoadFloat3(&m_keyFrames[(*currentKeyFrameIndex)].scale);
        rightScale = XMLoadFloat3(&m_keyFrames[(*currentKeyFrameIndex)+1].scale);
        scale = XMVectorLerp(leftScale, rightScale, ratio);

        leftRotation = XMLoadFloat4(&m_keyFrames[(*currentKeyFrameIndex)].rotation);
        rightRotation = XMLoadFloat4(&m_keyFrames[(*currentKeyFrameIndex) + 1].rotation);
        rotation = XMQuaternionSlerp(leftRotation, rightRotation, ratio);
        rotation = XMQuaternionNormalize(rotation);

        leftTranslation = XMVectorSetW(XMLoadFloat3(&m_keyFrames[(*currentKeyFrameIndex)].translation), 1.f);
        rightTranslation = XMVectorSetW(XMLoadFloat3(&m_keyFrames[(*currentKeyFrameIndex) + 1].translation), 1.f);
        translation = XMVectorLerp(leftTranslation, rightTranslation, ratio);
    }

    _matrix		boneTransformationMatrix = XMMatrixAffineTransformation(
        scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), rotation, translation);

    Bones[m_boneIndex]->Update_TransformationMatrix(boneTransformationMatrix);
}
void CChannel::Update_ToMatrix(
    _uint* currentKeyFrameIndex,
    _float currentTrackPosition,
    vector<_matrix>& outMatrices)
{
    if (0.f == currentTrackPosition)
        (*currentKeyFrameIndex) = 0;

    KeyFrame lastKeyFrame = m_keyFrames.back();

    _vector scale, rotation, translation;

    if (currentTrackPosition >= lastKeyFrame.trackPosition)
    {
        scale = XMLoadFloat3(&lastKeyFrame.scale);
        rotation = XMLoadFloat4(&lastKeyFrame.rotation);
        translation = XMVectorSetW(XMLoadFloat3(&lastKeyFrame.translation), 1.f);
    }
    else
    {
        while ((*currentKeyFrameIndex + 1) < m_numKeyFrames &&
            currentTrackPosition >= m_keyFrames[(*currentKeyFrameIndex) + 1].trackPosition)
        {
            ++(*currentKeyFrameIndex);
        }

        float t0 = m_keyFrames[*currentKeyFrameIndex].trackPosition;
        float t1 = m_keyFrames[*currentKeyFrameIndex + 1].trackPosition;

        float ratio = (t1 > t0) ? (currentTrackPosition - t0) / (t1 - t0) : 0.f;

        _vector s0 = XMLoadFloat3(&m_keyFrames[*currentKeyFrameIndex].scale);
        _vector s1 = XMLoadFloat3(&m_keyFrames[*currentKeyFrameIndex + 1].scale);
        scale = XMVectorLerp(s0, s1, ratio);

        _vector r0 = XMLoadFloat4(&m_keyFrames[*currentKeyFrameIndex].rotation);
        _vector r1 = XMLoadFloat4(&m_keyFrames[*currentKeyFrameIndex + 1].rotation);
        rotation = XMQuaternionNormalize(XMQuaternionSlerp(r0, r1, ratio));

        _vector p0 = XMVectorSetW(XMLoadFloat3(&m_keyFrames[*currentKeyFrameIndex].translation), 1.f);
        _vector p1 = XMVectorSetW(XMLoadFloat3(&m_keyFrames[*currentKeyFrameIndex + 1].translation), 1.f);
        translation = XMVectorLerp(p0, p1, ratio);
    }

    _matrix mat = XMMatrixAffineTransformation(
        scale,
        XMVectorZero(),
        rotation,
        translation
    );

    outMatrices[m_boneIndex] = mat;
}

shared_ptr<CChannel> CChannel::Create(ifstream& InFile)
{
    shared_ptr<CChannel> instance(new CChannel(), [](CChannel* p) {p->Free(); delete p; });

    if (FAILED(instance->Initialize(InFile)))
    {
        MSG_BOX("Failed to Created : CChannel");
    }
    return instance;
}


void CChannel::Free()
{
	__super::Free();
    m_keyFrames.clear();
}
