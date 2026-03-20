#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Bone.h"
#include "Converter_Struct.h"
#include "Animation.h"

CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& Prototype)
	:CComponent{ Prototype },
	m_eType{ Prototype.m_eType },
	m_PreLocalTransformMatrix{ Prototype.m_PreLocalTransformMatrix },
	m_iNumMeshes{ Prototype.m_iNumMeshes },
	m_Meshes{ Prototype.m_Meshes },
	m_iNumMaterials{ Prototype.m_iNumMaterials },
	m_Materials{ Prototype.m_Materials },
	//m_Bones{ Prototype.m_Bones },
	m_iNumAnimations{ Prototype.m_iNumAnimations }
	//m_Animations{ Prototype.m_Animations }

{
	for(auto& PrototypeAnim : Prototype.m_Animations)
	{
		m_Animations.push_back(PrototypeAnim->Clone());
	}
	for (auto& PrototypeBone : Prototype.m_Bones )
	{
		m_Bones.push_back(PrototypeBone->Clone());
	}

}

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix)
{
	ifstream InFile(pModelFilePath, ios::binary);
	if (!InFile.is_open()) return E_FAIL;

	Cvt_Header header;
	InFile.read(reinterpret_cast<_char*>(&header), sizeof(Cvt_Header));
	if (header.iMagic != 0x4D534842)
		return E_FAIL;

	m_eType = static_cast<MODEL>(header.bIsAnim);

	m_iNumMeshes = header.iNumMeshes;

	m_iNumMaterials = header.iNumMaterial;

	m_iNumAnimations = header.iNumAnimation;

	m_iTotalNumBone = header.iTotalNumBone;


	XMStoreFloat4x4(&m_PreLocalTransformMatrix, PreLocalTransformMatrix);

	/* 메시를 생성한다. */
	
		if (FAILED(Ready_Bones(InFile)))
			return E_FAIL;
	

	if (FAILED(Ready_Meshes(eType, InFile)))
		return E_FAIL;
	if (m_eType == MODEL::ANIM)
	{
		if (FAILED(Ready_Animations(InFile)))
			return E_FAIL;
	}
	if (FAILED(Ready_Material( InFile)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	//for (auto& pMesh : m_Meshes)
	//{
	m_Meshes[iMeshIndex]->Bind_Resources();
	m_Meshes[iMeshIndex]->Render();
	// }

	return S_OK;
}

HRESULT CModel::Ready_Meshes(MODEL eType, ifstream& InFile)
{

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{


		shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, eType, 
			InFile,
			static_pointer_cast<CModel>(shared_from_this()),
			XMLoadFloat4x4(&m_PreLocalTransformMatrix)
		);

		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Material( ifstream& InFile)
{

	/*for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		Cvt_Material mat;
		InFile.read(reinterpret_cast<_char*>(&mat), sizeof(Cvt_Material));

		shared_ptr<CMaterial> pMat = CMaterial::Create(m_pDevice, m_pContext, mat, pModelFilePath);
		if (nullptr == pMat)
			return E_FAIL;

		m_Materials.push_back(pMat);
	}*/

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		shared_ptr<CMaterial> pMat = CMaterial::Create(m_pDevice, m_pContext, InFile);
		if (nullptr == pMat)
			return E_FAIL;

		m_Materials.push_back(pMat);
	}

	return S_OK;
}

HRESULT CModel::Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex, Cvt_TexType eMaterialType,
	_uint iTextureIndex)
{
	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()]->Bind_Material(pShader, pConstantName, eMaterialType, iTextureIndex);
}

HRESULT CModel::Ready_Bones(ifstream& InFile)
{
	/*Cvt_Bone Bone = {};
	InFile.read((char*)&Bone,sizeof(Cvt_Bone) );


	shared_ptr<CBone> pBone = CBone::Create(Bone, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);*/

	//_int iPIndex = m_Bones.size() - 1; // 부모인덱스 = 백터 사이즈 -1 

	//for (_uint i = 0; i < pAINode->mNumChildren; i++)
	//{
	//	Ready_Bones(pAINode->mChildren[i], iPIndex);
	//}


	for (uint32_t i = 0; i < m_iTotalNumBone; ++i)
	{
		Cvt_Bone BoneDesc = {};

		
		InFile.read((char*)&BoneDesc, sizeof(Cvt_Bone));

		shared_ptr<CBone> pBone = CBone::Create(BoneDesc);

		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}
	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantNamem, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantNamem, m_Bones);
}

HRESULT CModel::Ready_Animations(ifstream& InFile)
{
	/*m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		shared_ptr<CAnimation> pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}*/


	for (_uint i = 0; i < m_iNumAnimations; i++)
	{

		shared_ptr<CAnimation> pAnimation = CAnimation::Create(InFile);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
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

_int CModel::Get_BoneIndex(const _char* pBoneName)
{
	_int iBoneIndex = -1;


	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone)->bool
		{
			++iBoneIndex;
			return pBone->isCompare(pBoneName);
		});

	if (iter == m_Bones.end())
	{
		return -1;
	}

	return iBoneIndex;
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName)
{
	auto iter = find_if(m_Bones.begin(),m_Bones.end(),
		[&](shared_ptr<CBone> pBone)->bool{
	
		return pBone->isCompare(pBoneName);
	});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrixPtr();

}

shared_ptr<CModel> CModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix)
{
	shared_ptr<CModel> pInstance(new CModel(pDevice, pContext), [](CModel* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eType, PreLocalTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
	}
	return pInstance;

}

shared_ptr<CComponent> CModel::Clone(void* pArg)
{

	shared_ptr<CComponent> pInstance(new CModel(*this), [](CModel* p) {p->Free(); delete p; });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	m_Animations.clear();

	m_Materials.clear();

	m_Meshes.clear();

	m_Bones.clear();


}
