#include "Assimp_Bone.h"

CAssimp_Bone::CAssimp_Bone()
{
}

HRESULT CAssimp_Bone::Initialize(const aiNode* srcAINode, _int parentBoneIndex)
{
    strcpy_s(m_name, srcAINode->mName.data);

    m_parentBoneIndex = parentBoneIndex;

    memcpy(&m_transformationMatrix, &srcAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&m_transformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_transformationMatrix)));
    // 전치 필수

    XMStoreFloat4x4(&m_combinedTransformationMatrix, XMMatrixIdentity());

    return S_OK;
}

void CAssimp_Bone::Update_CombinedTransformMatrix(const vector<shared_ptr<CAssimp_Bone>> Bones, _fmatrix preTransformMatrix)
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

void CAssimp_Bone::Update_TransformationMatrix(_fmatrix transformMatrix)
{
    XMStoreFloat4x4(&m_transformationMatrix, transformMatrix);
}


shared_ptr<CAssimp_Bone> CAssimp_Bone::Create(const aiNode* srcAINode, _int parentBoneIndex)
{
    shared_ptr<CAssimp_Bone> instance(new CAssimp_Bone(), [](CAssimp_Bone* p) {p->Free(); delete p; });

    if (FAILED(instance->Initialize(srcAINode, parentBoneIndex)))
    {
        MSG_BOX("Failed to Created : CMesh");
    }
    return instance;

}

void CAssimp_Bone::Free()
{
	__super::Free();
}
