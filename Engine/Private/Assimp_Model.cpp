#include "Assimp_Model.h"
#include "Assimp_Mesh.h"
#include "Assimp_Material.h"
#include "Assimp_Bone.h"
#include "Assimp_Animation.h"

CAssimp_Model::CAssimp_Model(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    : CComponent(device, context)
{
}

CAssimp_Model::CAssimp_Model(const CAssimp_Model& Prototype)
    :CComponent{Prototype},
    m_AIScene{ Prototype.m_AIScene }, 
    m_type{Prototype.m_type},
    m_preLocalTransformMatrix{ Prototype.m_preLocalTransformMatrix },
    m_numMeshes{ Prototype.m_numMeshes },
    m_meshes{Prototype.m_meshes},
    m_numMaterials{ Prototype.m_numMaterials },
    m_materials{ Prototype.m_materials },
	m_bones{Prototype.m_bones},
    m_numAnimations{ Prototype.m_numAnimations },
    m_animations{ Prototype.m_animations }

{
}

HRESULT CAssimp_Model::Initialize_Prototype(const _char* modelFilePath, EModelType type, _fmatrix preLocalTransformMatrix)
{
    _uint flag = { /*aiProcess_GlobalScale | aiProcess_PreTransformVertices |*/ aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (EModelType::NONANIM == type)
    {
        flag |= aiProcess_PreTransformVertices;
    }

    // 로드해야할 모든 데이터를 다 읽어들이고 aiScene이라는 타입안에 보관된다. 
    m_AIScene = m_importer.ReadFile(modelFilePath, flag);
    if (nullptr == m_AIScene)
        return E_FAIL;


    m_type = type;
    XMStoreFloat4x4(&m_preLocalTransformMatrix, preLocalTransformMatrix);

    /* aiScene안의 데이터를 DX환경에서 이용하기 쉽도록 정리하는 작업 수행.  */

        /* 메시를 생성한다. */
    if (FAILED(Ready_Bones(m_AIScene->mRootNode, -1)))
        return E_FAIL;
    if (FAILED(Ready_Meshes(type)))
        return E_FAIL;
    if (FAILED(Ready_Animations()))
        return E_FAIL;
    if (FAILED(Ready_Material(modelFilePath)))
        return E_FAIL;
    return S_OK;
}

HRESULT CAssimp_Model::Initialize(void* arg)
{
    return S_OK;
}

HRESULT CAssimp_Model::Render(_uint meshIndex)
{
    //for (auto& pMesh : m_Meshes)
    //{
    m_meshes[meshIndex]->Bind_Resources();
    m_meshes[meshIndex]->Render();
   // }

    return S_OK;
}

HRESULT CAssimp_Model::Ready_Meshes(EModelType type)
{
    m_numMeshes = m_AIScene->mNumMeshes;

    for (_uint i = 0; i < m_numMeshes; i++)
    {
        shared_ptr<CAssimp_Mesh> mesh = CAssimp_Mesh::Create(m_device, m_context,
            type, m_AIScene->mMeshes[i],
            static_pointer_cast<CAssimp_Model>(shared_from_this()),
            XMLoadFloat4x4(&m_preLocalTransformMatrix));

        if (nullptr == mesh)
            return E_FAIL;

        m_meshes.push_back(mesh);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Ready_Material(const _char* modelFilePath)
{
    m_numMaterials = m_AIScene->mNumMaterials;

    for (_uint i = 0; i < m_numMaterials; i++)
    {
        shared_ptr<CAssimp_Material> mat = CAssimp_Material::Create(m_device, m_context, m_AIScene->mMaterials[i], modelFilePath);
        if (nullptr == mat)
            return E_FAIL;

        m_materials.push_back(mat);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Bind_Material(shared_ptr<CShader> shader,  const _char* constantName, _uint meshIndex, aiTextureType materialType,
                              _uint textureIndex)
{
    return m_materials[m_meshes[meshIndex]->Get_MaterialIndex()]->Bind_Material(shader, constantName, materialType, textureIndex);
}



HRESULT CAssimp_Model::Ready_Bones(const aiNode* srcAINode, _int parentIndex)
{
    shared_ptr<CAssimp_Bone> bone = CAssimp_Bone::Create(srcAINode, parentIndex);
    if (nullptr == bone)
        return E_FAIL;

	m_bones.push_back(bone);

    _int curBoneIndex = m_bones.size() - 1; // 부모인덱스 = 백터 사이즈 -1 

    for (_uint i = 0; i < srcAINode->mNumChildren; i++)
    {
        Ready_Bones(srcAINode->mChildren[i], curBoneIndex);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Bind_BoneMatrices(shared_ptr<CShader> shader, const _char* constantNamem, _uint meshIndex)
{
    return m_meshes[meshIndex]->Bind_BoneMatrices(shader, constantNamem, m_bones);
}

HRESULT CAssimp_Model::Ready_Animations()
{
    m_numAnimations = m_AIScene->mNumAnimations;

    for (_uint i = 0; i < m_numAnimations; i++)
    {
        shared_ptr<CAssimp_Animation> animation = CAssimp_Animation::Create(m_AIScene->mAnimations[i], static_pointer_cast<CAssimp_Model>(shared_from_this()));
        if (nullptr == animation)
            return E_FAIL;

        m_animations.push_back(animation);
    }

    return S_OK;
}

HRESULT CAssimp_Model::Play_Animation(_float timeDelta)
{
    // 현제 애니메이션에 맞는 상태대로 뼈의 Transformation을 갱신해준다
    _bool       isFinish = { false };
    isFinish = m_animations[m_currentAnimIndex]->Update_TransformationMatrices(timeDelta, m_bones, m_isAnimLoop);


    // 모든뼈를 순회하면 CombinedTransformation을 셋팅 해준다

    for (auto& bone : m_bones)
    {
        bone->Update_CombinedTransformMatrix(m_bones, XMLoadFloat4x4(&m_preLocalTransformMatrix));
    }

    return S_OK;
}

_int CAssimp_Model::Get_BoneIndex(const _char* boneName)
{
    _int boneIndex = -1;


    auto iter = find_if(m_bones.begin(), m_bones.end(), [&](shared_ptr<CAssimp_Bone> bone)->bool
    {
            ++boneIndex;
            return bone->isCompare(boneName);
    });

    if(iter == m_bones.end())
    {
        return -1;
    }

    return boneIndex;
}

shared_ptr<CAssimp_Model> CAssimp_Model::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _char* modelFilePath, EModelType type, _fmatrix preLocalTransformMatrix)
{
    shared_ptr<CAssimp_Model> instance(new CAssimp_Model(device, context), [](CAssimp_Model* p) {p->Free(); delete p;});

    if (FAILED(instance->Initialize_Prototype(modelFilePath, type, preLocalTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CAssimp_Model");
    }
    return instance;

}

shared_ptr<CComponent> CAssimp_Model::Clone(void* arg)
{

    shared_ptr<CComponent> instance(new CAssimp_Model(*this), [](CAssimp_Model* p) {p->Free(); delete p;});

    if (FAILED(instance->Initialize(arg)))
    {
        MSG_BOX("Failed to Cloned : CAssimp_Model");
    }
    return instance;
}

void CAssimp_Model::Free()
{
    __super::Free();

    m_meshes.clear();

    m_importer.FreeScene();
}
