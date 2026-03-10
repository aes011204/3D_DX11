#include "Model.h"
#include "Mesh.h"

CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& Prototype)
    :CComponent{Prototype},
    m_pAIScene{ Prototype.m_pAIScene }, 
    m_eType{Prototype.m_eType},
    m_PreLocalTransformMatrix{ Prototype.m_PreLocalTransformMatrix },
    m_iNumMeshes{ Prototype.m_iNumMeshes },
    m_Meshes{Prototype.m_Meshes}

{
}

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix)
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
    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Render()
{
    for (auto& pMesh : m_Meshes)
    {
        pMesh->Bind_Resources();
        pMesh->Render();
    }

    return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreLocalTransformMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

shared_ptr<CModel> CModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix)
{
    shared_ptr<CModel> pInstance(new CModel(pDevice, pContext), [](CModel* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eType, PreLocalTransformMatrix)))
    {
        MSG_BOX("Failed to Created : CModel");
    }
    return pInstance;

}

shared_ptr<CComponent> CModel::Clone(void* pArg)
{

    shared_ptr<CComponent> pInstance(new CModel(*this), [](CModel* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CModel");
    }
    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    m_Meshes.clear();

    m_Importer.FreeScene();
}
