#include "Channel.h"

#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}


HRESULT CChannel::Initialize(ifstream& InFile)
{

    Cvt_Channel ChannelDesc = {};
    InFile.read((char*)&ChannelDesc, sizeof(Cvt_Channel));




    m_iBoneIndex = ChannelDesc.iBoneIndex;

    m_iNumKeyFrames = ChannelDesc.iNumKeyframes;

    m_KeyFrames.reserve(m_iNumKeyFrames); // 미리 공간 확보

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        Cvt_Keyframe KeyFrameDesc = {};
        InFile.read((char*)&KeyFrameDesc, sizeof(Cvt_Keyframe));

        KEYFRAME KeyFrame = {};
        // 실제 컨버터에서 저장한 시간을 트랙 포지션으로 사용
        KeyFrame.fTrackPosition = (float)KeyFrameDesc.dTrackPosition;

        // 데이터 복사 (Cvt_Keyframe -> KEYFRAME)
        memcpy(&KeyFrame.vScale, KeyFrameDesc.vScale, sizeof(_float3));
        memcpy(&KeyFrame.vRotation, KeyFrameDesc.qRotation, sizeof(_float4)); // qRot 사용
        memcpy(&KeyFrame.vTranslation, KeyFrameDesc.vPos, sizeof(_float3)); // vPos 사용

        m_KeyFrames.push_back(KeyFrame);
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

void CChannel::Update_TransformationMatrix(_uint* pCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<shared_ptr<CBone>>& Bones)
{
    if (0.f == fCurrentTrackPosition)
        (*pCurrentKeyFrameIndex) = 0;

    KEYFRAME LastKeyFrame = m_KeyFrames.back();

    _vector vScale, vRotation, vTranslation;

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)// 마지막 프레임 이후 그 자세 그대로 유지
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    else // 사이보간
    {
        while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
            ++(*pCurrentKeyFrameIndex);

        _float fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
            (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

        _vector vLeftScale, vRightScale;
        _vector	vLeftRotation, vRightRotation;
        _vector	vLeftTranslation, vRightTranslation;

        vLeftScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale);
        vRightScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)+1].vScale);
        vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);

        vLeftRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation);
        vRightRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation);
        vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);
        vRotation = XMQuaternionNormalize(vRotation);

        vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);
        vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), 1.f);
        vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);
    }

    _matrix		BoneTransformationMatrix = XMMatrixAffineTransformation(
        vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Update_TransformationMatrix(BoneTransformationMatrix);
}
void CChannel::Update_ToMatrix(
    _uint* pCurrentKeyFrameIndex,
    _float fCurrentTrackPosition,
    vector<_matrix>& OutMatrices)
{
    if (0.f == fCurrentTrackPosition)
        (*pCurrentKeyFrameIndex) = 0;

    KEYFRAME LastKeyFrame = m_KeyFrames.back();

    _vector vScale, vRotation, vTranslation;

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    else
    {
        while ((*pCurrentKeyFrameIndex + 1) < m_iNumKeyFrames &&
            fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
        {
            ++(*pCurrentKeyFrameIndex);
        }

        float t0 = m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition;
        float t1 = m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition;

        float ratio = (t1 > t0) ? (fCurrentTrackPosition - t0) / (t1 - t0) : 0.f;

        _vector s0 = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
        _vector s1 = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
        vScale = XMVectorLerp(s0, s1, ratio);

        _vector r0 = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
        _vector r1 = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
        vRotation = XMQuaternionNormalize(XMQuaternionSlerp(r0, r1, ratio));

        _vector p0 = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
        _vector p1 = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);
        vTranslation = XMVectorLerp(p0, p1, ratio);
    }

    _matrix mat = XMMatrixAffineTransformation(
        vScale,
        XMVectorZero(),
        vRotation,
        vTranslation
    );

    OutMatrices[m_iBoneIndex] = mat;
}

shared_ptr<CChannel> CChannel::Create(ifstream& InFile)
{
    shared_ptr<CChannel> pInstance(new CChannel(), [](CChannel* p) {p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(InFile)))
    {
        MSG_BOX("Failed to Created : CChannel");
    }
    return pInstance;
}


void CChannel::Free()
{
	__super::Free();
    m_KeyFrames.clear();
}
