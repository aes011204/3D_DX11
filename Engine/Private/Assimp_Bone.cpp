#include "Assimp_Bone.h"

CAssimp_Bone::CAssimp_Bone()
{
}

HRESULT CAssimp_Bone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
    strcpy_s(m_szName, pAINode->mName.data);

    m_iParentBoneIndex = iParentBoneIndex;

    memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    // 전치 필수

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CAssimp_Bone::Update_CombinedTransformMatrix(const vector<shared_ptr<CAssimp_Bone>> Bones, _fmatrix PreTransformMatrix)
{
    if(-1 == m_iParentBoneIndex)
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
    }
    else
    {
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
    }
}

void CAssimp_Bone::Update_TransformationMatrix(_fmatrix TransformMatrix)
{
    XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);
}


shared_ptr<CAssimp_Bone> CAssimp_Bone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
    shared_ptr<CAssimp_Bone> pInstance(new CAssimp_Bone(), [](CAssimp_Bone* p) {p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
    {
        MSG_BOX("Failed to Created : CMesh");
    }
    return pInstance;

}

void CAssimp_Bone::Free()
{
	__super::Free();
}
