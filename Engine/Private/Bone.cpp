#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const Cvt_Bone& BoneDesc)
{
    strcpy_s(m_name, BoneDesc.name);

    m_parentBoneIndex = BoneDesc.parentBoneIndex;

    memcpy(&m_transformationMatrix, &BoneDesc.transformationMatrix, sizeof(_float4x4));
   // XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    // 전치 필수 -> 내부에서 햇음 

    XMStoreFloat4x4(&m_combinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CBone::Update_CombinedTransformMatrix(const vector<shared_ptr<CBone>> Bones, _fmatrix PreTransformMatrix)
{
    if(-1 == m_parentBoneIndex)
    {
        XMStoreFloat4x4(&m_combinedTransformationMatrix,
            XMLoadFloat4x4(&m_transformationMatrix) * PreTransformMatrix);
    }
    else
    {
        XMStoreFloat4x4(&m_combinedTransformationMatrix,
            XMLoadFloat4x4(&m_transformationMatrix) * XMLoadFloat4x4(&Bones[m_parentBoneIndex]->m_combinedTransformationMatrix));
    }
}

void CBone::Update_TransformationMatrix(_fmatrix TransformMatrix)
{
    XMStoreFloat4x4(&m_transformationMatrix, TransformMatrix);
}


shared_ptr<CBone> CBone::Create(const Cvt_Bone& BoneDesc)
{
    shared_ptr<CBone> pInstance(new CBone(), [](CBone* p) {p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(BoneDesc)))
    {
        MSG_BOX("Failed to Created : CBone");
    }
    return pInstance;

}

shared_ptr<CBone> CBone::Clone()
{

        shared_ptr<CBone> pInstance(new CBone(*this), [](CBone* p) {p->Free(); delete p; });
        return pInstance;
    

}

void CBone::Free()
{
	__super::Free();
}
