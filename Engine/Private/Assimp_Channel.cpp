#include "Assimp_Channel.h"

#include "Assimp_Model.h"
#include "Assimp_Bone.h"

CAssimp_Channel::CAssimp_Channel()
{
}


HRESULT CAssimp_Channel::Initialize(const aiNodeAnim* pAIChannel, shared_ptr<class CAssimp_Model> pModel)
{
    m_boneIndex = pModel->Get_BoneIndex(pAIChannel->mNodeName.data);

	m_numKeyFrames = max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys);
	m_numKeyFrames = max(m_numKeyFrames, pAIChannel->mNumScalingKeys);

    _float3 vScale = {};
    _float4 vRotation = {};
    _float3 vTranslation = {};

    for(size_t i =0;i<m_numKeyFrames;i++)
    {
        KeyFrame keyFrame = {};
        if(pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            keyFrame.trackPosition = pAIChannel->mScalingKeys[i].mTime; // 이거 실제 시간이 아님
        }
        if (pAIChannel->mNumRotationKeys > i)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

            keyFrame.trackPosition = pAIChannel->mRotationKeys[i].mTime;
        }
        if (pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            keyFrame.trackPosition = pAIChannel->mPositionKeys[i].mTime; // 이거 실제 시간이 아님
        }

        keyFrame.scale = vScale;
        keyFrame.rotation = vRotation;
        keyFrame.translation = vTranslation;

        m_keyFrames.push_back(keyFrame);
    }

     
    return S_OK;
}

void CAssimp_Channel::Update_TransformationMatrix(_float fCurrentTrackPosition, const vector<shared_ptr<CAssimp_Bone>>& Bones)
{
    if (0.f == fCurrentTrackPosition)
        m_currentKeyFrameIndex = 0;

    KeyFrame LastKeyFrame = m_keyFrames.back();

    _vector vScale, vRotation, vTranslation;

    if (fCurrentTrackPosition >= LastKeyFrame.trackPosition)// 마지막 프레임 이후 그 자세 그대로 유지
    {
        vScale = XMLoadFloat3(&LastKeyFrame.scale);
        vRotation = XMLoadFloat4(&LastKeyFrame.rotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.translation), 1.f);
    }
    else // 사이보간
    {
        while (fCurrentTrackPosition >= m_keyFrames[m_currentKeyFrameIndex + 1].trackPosition)
            ++m_currentKeyFrameIndex;

        _float fRatio = (fCurrentTrackPosition - m_keyFrames[m_currentKeyFrameIndex].trackPosition) /
            (m_keyFrames[m_currentKeyFrameIndex + 1].trackPosition - m_keyFrames[m_currentKeyFrameIndex].trackPosition);

        _vector vLeftScale, vRightScale;
        _vector	vLeftRotation, vRightRotation;
        _vector	vLeftTranslation, vRightTranslation;

        vLeftScale = XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex].scale);
        vRightScale = XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex+1].scale);
        vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);

        vLeftRotation = XMLoadFloat4(&m_keyFrames[m_currentKeyFrameIndex].rotation);
        vRightRotation = XMLoadFloat4(&m_keyFrames[m_currentKeyFrameIndex + 1].rotation);
        vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);

        vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex].translation), 1.f);
        vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_keyFrames[m_currentKeyFrameIndex + 1].translation), 1.f);
        vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);
    }

    _matrix		BoneTransformationMatrix = XMMatrixAffineTransformation(
        vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_boneIndex]->Update_TransformationMatrix(BoneTransformationMatrix);
}

shared_ptr<CAssimp_Channel> CAssimp_Channel::Create(const aiNodeAnim* pAIChannel, shared_ptr<class CAssimp_Model> pModel)
{
    shared_ptr<CAssimp_Channel> pInstance(new CAssimp_Channel(), [](CAssimp_Channel* p) {p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
    {
        MSG_BOX("Failed to Created : CAssimp_Channel");
    }
    return pInstance;
}


void CAssimp_Channel::Free()
{
	__super::Free();
    m_keyFrames.clear();
}
