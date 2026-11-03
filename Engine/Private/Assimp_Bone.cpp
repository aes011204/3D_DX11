#include "Assimp_Bone.h"

CAssimp_Bone::CAssimp_Bone()
{
}

HRESULT CAssimp_Bone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
    strcpy_s(m_name, pAINode->mName.data);

    m_parentBoneIndex = iParentBoneIndex;

    memcpy(&m_transformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&m_transformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_transformationMatrix)));
    // 전치 필수

    XMStoreFloat4x4(&m_combinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CAssimp_Bone::Update_CombinedTransformMatrix(const vector<shared_ptr<CAssimp_Bone>> Bones, _fmatrix PreTransformMatrix)
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

void CAssimp_Bone::Update_TransformationMatrix(_fmatrix TransformMatrix)
{
    XMStoreFloat4x4(&m_transformationMatrix, TransformMatrix);
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
