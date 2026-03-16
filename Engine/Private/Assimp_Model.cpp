#include "Assimp_Model.h"
#include "Assimp_Mesh.h"
#include "Assimp_Material.h"
#include "Assimp_Bone.h"
#include "Assimp_Animation.h"

CAssimp_Model::CAssimp_Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CAssimp_Model::CAssimp_Model(const CAssimp_Model& Prototype)
    :CComponent{Prototype},
    m_pAIScene{ Prototype.m_pAIScene }, 
    m_eType{Prototype.m_eType},
    m_PreLocalTransformMatrix{ Prototype.m_PreLocalTransformMatrix },
    m_iNumMeshes{ Prototype.m_iNumMeshes },
    m_Meshes{Prototype.m_Meshes},
    m_iNumMaterials{ Prototype.m_iNumMaterials },
    m_Materials{ Prototype.m_Materials },
	m_Bones{Prototype.m_Bones},
    m_iNumAnimations{ Prototype.m_iNumAnimations },
    m_Animations{ Prototype.m_Animations }

{
}

HRESULT CAssimp_Model::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix)
{
    _uint iFlag = { /*aiProcess_GlobalScale | aiProcess_PreTransformVertices |*/ aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (MODEL::NONANIM == eType)
    {
        iFlag |= aiProcess_PreTransformVertices;
    }

    // 로드해야할 모든 데이터를 다 읽어들이고 aiScene이라는 타입안에 보관된다. 
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;


    m_eType = eType;
    XMStoreFloat4x4(&m_PreLocalTransformMatrix, PreLocalTransformMatrix);

    /* aiScene안의 데이터를 DX환경에서 이용하기 쉽도록 정리하는 작업 수행.  */

        /* 메시를 생성한다. */
    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;
    if (FAILED(Ready_Meshes(eType)))
        return E_FAIL;
    if (FAILED(Ready_Material(pModelFilePath)))
        return E_FAIL;
    if (FAILED(Ready_Animations()))
        return E_FAIL;
    return S_OK;
}

HRESULT CAssimp_Model::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CAssimp_Model::Render(_uint iMeshIndex)
{
    //for (auto& pMesh : m_Meshes)
    //{
    m_Meshes[iMeshIndex]->Bind_Resources();
    m_Meshes[iMeshIndex]->Render();
   // }

    return S_OK;
}

HRESULT CAssimp_Model::Ready_Meshes(MODEL eType)
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        shared_ptr<CAssimp_Mesh> pMesh = CAssimp_Mesh::Create(m_pDevice, m_pContext,
            eType, m_pAIScene->mMeshes[i],
            static_pointer_cast<CAssimp_Model>(shared_from_this()),
            XMLoadFloat4x4(&m_PreLocalTransformMatrix));

        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Ready_Material(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (_uint i = 0; i < m_iNumMaterials; i++)
    {
        shared_ptr<CAssimp_Material> pMat = CAssimp_Material::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], pModelFilePath);
        if (nullptr == pMat)
            return E_FAIL;

        m_Materials.push_back(pMat);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Bind_Material(shared_ptr<CShader> pShader,  const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType,
                              _uint iTextureIndex)
{
    return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()]->Bind_Material(pShader, pConstantName, eMaterialType, iTextureIndex);
}



HRESULT CAssimp_Model::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    shared_ptr<CAssimp_Bone> pBone = CAssimp_Bone::Create(pAINode, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

	m_Bones.push_back(pBone);

    _int iPIndex = m_Bones.size() - 1; // 부모인덱스 = 백터 사이즈 -1 

    for (_uint i = 0; i < pAINode->mNumChildren; i++)
    {
        Ready_Bones(pAINode->mChildren[i], iPIndex);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantNamem, _uint iMeshIndex)
{
    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantNamem, m_Bones);
}

HRESULT CAssimp_Model::Ready_Animations()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (_uint i = 0; i < m_iNumAnimations; i++)
    {
        shared_ptr<CAssimp_Animation> pAnimation = CAssimp_Animation::Create(m_pAIScene->mAnimations[i], static_pointer_cast<CAssimp_Model>(shared_from_this()));
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Play_Animation(_float fTimeDelta)
{
    // 현제 애니메이션에 맞는 상태대로 뼈의 Transformation을 갱신해준다
    _bool       isFinish = { false };
    isFinish = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones, m_isAnimLoop);


    // 모든뼈를 순회하면 CombinedTransformation을 셋팅 해준다

    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformMatrix(m_Bones, XMLoadFloat4x4(&m_PreLocalTransformMatrix));
    }

    return S_OK;
}

_int CAssimp_Model::Get_BoneIndex(const _char* pBoneName)
{
    _int iBoneIndex = -1;


    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CAssimp_Bone> pBone)->bool
    {
            ++iBoneIndex;
            return pBone->isCompare(pBoneName);
    });

    if(iter == m_Bones.end())
    {
        return -1;
    }

    return iBoneIndex;
}

shared_ptr<CAssimp_Model> CAssimp_Model::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix)
{
    shared_ptr<CAssimp_Model> pInstance(new CAssimp_Model(pDevice, pContext), [](CAssimp_Model* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eType, PreLocalTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CAssimp_Model");
    }
    return pInstance;

}

shared_ptr<CComponent> CAssimp_Model::Clone(void* pArg)
{

    shared_ptr<CComponent> pInstance(new CAssimp_Model(*this), [](CAssimp_Model* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAssimp_Model");
    }
    return pInstance;
}

void CAssimp_Model::Free()
{
    __super::Free();

    m_Meshes.clear();

    m_Importer.FreeScene();
}
