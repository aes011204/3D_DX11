#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const Cvt_Bone& boneDesc)
{
    strcpy_s(m_name, boneDesc.name);

    m_parentBoneIndex = boneDesc.parentBoneIndex;

    memcpy(&m_transformationMatrix, &boneDesc.transformationMatrix, sizeof(_float4x4));
   // XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    // 전치 필수 -> 내부에서 햇음 

    XMStoreFloat4x4(&m_combinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CBone::Update_CombinedTransformMatrix(const vector<shared_ptr<CBone>> Bones, _fmatrix preTransformMatrix)
{
    if(-1 == m_parentBoneIndex)
    {
        XMStoreFloat4x4(&m_combinedTransformationMatrix,
            XMLoadFloat4x4(&m_transformationMatrix) * preTransformMatrix);
    }
    else
    {
        XMStoreFloat4x4(&m_combinedTransformationMatrix,
            XMLoadFloat4x4(&m_transformationMatrix) * XMLoadFloat4x4(&Bones[m_parentBoneIndex]->m_combinedTransformationMatrix));
    }
}

void CBone::Update_TransformationMatrix(_fmatrix transformMatrix)
{
    XMStoreFloat4x4(&m_transformationMatrix, transformMatrix);
}


shared_ptr<CBone> CBone::Create(const Cvt_Bone& boneDesc)
{
    shared_ptr<CBone> instance(new CBone(), [](CBone* p) {p->Free(); delete p; });

    if (FAILED(instance->Initialize(boneDesc)))
    {
        MSG_BOX("Failed to Created : CBone");
    }
    return instance;

}

shared_ptr<CBone> CBone::Clone()
{

        shared_ptr<CBone> instance(new CBone(*this), [](CBone* p) {p->Free(); delete p; });
        return instance;
    

}

void CBone::Free()
{
	__super::Free();
}
