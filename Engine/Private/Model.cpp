#include "Model.h"
#include "Mesh.h"

CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& Prototype)
    :CComponent{Prototype},
    m_iNumMeshes{Prototype.m_iNumMeshes},
    m_Meshes{Prototype.m_Meshes},
    m_pAIScene{ Prototype.m_pAIScene}

{
}

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath)
{
    _uint iFlag = { aiProcess_GlobalScale | aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    // 로드해야할 모든 데이터를 다 읽어들이고 aiScene이라는 타입안에 보관된다. 
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    /* aiScene안에 분포된데이터들을 내가 DX환경에서 이용하기 쉽도록 정리해주는 작업을 수행한다.  */

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
        shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i]);
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

shared_ptr<CModel> CModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath)
{
    shared_ptr<CModel> pInstance(new CModel(pDevice, pContext), [](CModel* p) {p->Free(); delete p;});

    if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
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
