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
	m_type{ Prototype.m_type },
	m_preLocalTransformMatrix{ Prototype.m_preLocalTransformMatrix },
	m_numMeshes{ Prototype.m_numMeshes },
	m_meshes{ Prototype.m_meshes },
	m_numMaterials{ Prototype.m_numMaterials },
	m_materials{ Prototype.m_materials },
	//m_Bones{ Prototype.m_Bones },
	m_numAnimations{ Prototype.m_numAnimations }
	,m_totalNumBone {Prototype.m_totalNumBone}
	//m_Animations{ Prototype.m_Animations }

{
	for(auto& PrototypeAnim : Prototype.m_animations)
	{
		m_animations.push_back(PrototypeAnim->Clone());
	}
	for (auto& PrototypeBone : Prototype.m_bones )
	{
		m_bones.push_back(PrototypeBone->Clone());
	}

}

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath, EModelType eType, _fmatrix PreLocalTransformMatrix)
{
	ifstream InFile(pModelFilePath, ios::binary);
	if (!InFile.is_open()) return E_FAIL;

	Cvt_Header header;
	InFile.read(reinterpret_cast<_char*>(&header), sizeof(Cvt_Header));
	if (header.magic != 0x4D534842)
		return E_FAIL;

	m_type = static_cast<EModelType>(header.isAnim);

	m_numMeshes = header.numMeshes;

	m_numMaterials = header.numMaterial;

	m_numAnimations = header.numAnimation;

	m_totalNumBone = header.totalNumBone;


	XMStoreFloat4x4(&m_preLocalTransformMatrix, PreLocalTransformMatrix);

	/* 메시를 생성한다. */
	
		if (FAILED(Ready_Bones(InFile)))
			return E_FAIL;
	

	if (FAILED(Ready_Meshes(eType, InFile)))
		return E_FAIL;
	if (m_type == EModelType::ANIM)
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
	m_meshes[iMeshIndex]->Bind_Resources();
	m_meshes[iMeshIndex]->Render();
	// }

	return S_OK;
}

HRESULT CModel::Ready_Meshes(EModelType eType, ifstream& InFile)
{

	for (_uint i = 0; i < m_numMeshes; i++)
	{


		shared_ptr<CMesh> pMesh = CMesh::Create(m_device, m_context, eType, 
			InFile,
			static_pointer_cast<CModel>(shared_from_this()),
			XMLoadFloat4x4(&m_preLocalTransformMatrix)
		);

		if (nullptr == pMesh)
			return E_FAIL;

		m_meshes.push_back(pMesh);
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

	for (_uint i = 0; i < m_numMaterials; ++i)
	{
		shared_ptr<CMaterial> pMat = CMaterial::Create(m_device, m_context, InFile);
		if (nullptr == pMat)
			return E_FAIL;

		m_materials.push_back(pMat);
	}

	return S_OK;
}

HRESULT CModel::Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex, Cvt_TexType eMaterialType,
	_uint iTextureIndex)
{
	return m_materials[m_meshes[iMeshIndex]->Get_MaterialIndex()]->Bind_Material(pShader, pConstantName, eMaterialType, iTextureIndex);
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


	for (uint32_t i = 0; i < m_totalNumBone; ++i)
	{
 		Cvt_Bone BoneDesc = {};

		
		InFile.read((char*)&BoneDesc, sizeof(Cvt_Bone));

		shared_ptr<CBone> pBone = CBone::Create(BoneDesc);

		if (nullptr == pBone)
			return E_FAIL;

		m_bones.push_back(pBone);
	}
	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantNamem, _uint iMeshIndex)
{
	return m_meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantNamem, m_bones);
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


	for (_uint i = 0; i < m_numAnimations; i++)
	{

		shared_ptr<CAnimation> pAnimation = CAnimation::Create(InFile);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_animations.push_back(pAnimation);
	}
	return S_OK;
}


HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	m_isFinish = { false };
	//if (m_IsBlending == true)
	//{
	//	m_fBlendElapsed += fTimeDelta;
	//	_float ratio = m_fBlendElapsed / m_fBlendTime;
	//	
	//	if (ratio >= 1.f)
	//	{
	//		m_IsBlending = false;
	//		m_iCurrentAnimIndex = m_BlendIndex;
	//		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones, true);
	//	}
	//	else
	//	{
	//		m_isFinish = m_Animations[m_iCurrentAnimIndex]->Blend_TransformationMatrices(fTimeDelta, m_Animations[m_BlendIndex], ratio, m_Bones, m_isAnimLoop, m_BlendIsLoop);
	//	}
	//}
	//else
	{
	m_isFinish = m_animations[m_currentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_bones, m_isAnimLoop);
	
	}
	// 현제 애니메이션에 맞는 상태대로 뼈의 Transformation을 갱신해준다


	// 모든뼈를 순회하면 CombinedTransformation을 셋팅 해준다

	for (auto& pBone : m_bones)
	{
		pBone->Update_CombinedTransformMatrix(m_bones, XMLoadFloat4x4(&m_preLocalTransformMatrix));
	}

	return S_OK;
}

void CModel::Set_Animation(_uint iIndex, _bool isLoop)
{

	//if (m_iCurrentAnimIndex != iIndex)
	//{
	//	m_IsBlending = true;
	//	m_fBlendElapsed = 0.f;
	//	m_BlendIndex = iIndex;
	//	m_BlendIsLoop = isLoop;
	//}

	m_currentAnimIndex = iIndex;

	m_isAnimLoop = isLoop;
	if(m_isAnimLoop==false)
	m_animations[m_currentAnimIndex]->ReStart();


	m_prevFrame = 0;
}

_int CModel::Get_BoneIndex(const _char* pBoneName)
{
	_int iBoneIndex = -1;


	auto iter = find_if(m_bones.begin(), m_bones.end(), [&](shared_ptr<CBone> pBone)->bool
		{
			++iBoneIndex;
			return pBone->isCompare(pBoneName);
		});

	if (iter == m_bones.end())
	{
		return -1;
	}

	return iBoneIndex;
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName)
{
	auto iter = find_if(m_bones.begin(),m_bones.end(),
		[&](shared_ptr<CBone> pBone)->bool{
	
		return pBone->isCompare(pBoneName);
	});

	if (iter == m_bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrixPtr();

}


_bool CModel::Check_AnimFrame(_int targetFrame)
{

	auto anim = m_animations[m_currentAnimIndex];

	_int curFrame = anim->Get_CurrentFrame();

	// 루프 처리
	if (curFrame < m_prevFrame)
		m_prevFrame = 0;

	if (m_prevFrame < targetFrame && curFrame >= targetFrame)
	{
		m_prevFrame = curFrame;
		return true;
	}

	m_prevFrame = curFrame;
	return false;
}

_int CModel::Get_CurrentFrame() const
{
	{
		return m_animations[m_currentAnimIndex]->Get_CurrentFrame();
	}
}

shared_ptr<CModel> CModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath, EModelType eType, _fmatrix PreLocalTransformMatrix)
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

	m_animations.clear();

	m_materials.clear();

	m_meshes.clear();

	m_bones.clear();


}
