#pragma once
#include "Component.h"
// 모델은 메쉬로 이루어져있음 
// 컴포넌트로 일단 상속 받음 

NS_BEGIN(Engine)

class CShader;

	class ENGINE_DLL CAssimp_Model final:
    public CComponent
{
protected:
	CAssimp_Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAssimp_Model(const CAssimp_Model& Prototype);
public:
	virtual ~CAssimp_Model() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, EModelType eType, _fmatrix PreLocalTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

	HRESULT Ready_Meshes(EModelType eType);

	HRESULT Ready_Material(const _char* pModelFilePath);
	HRESULT Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex,
	                      aiTextureType eMaterialType, _uint iTextureIndex);

	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantNamem, _uint iMeshIndex);

	HRESULT Ready_Animations();
	HRESULT Play_Animation(_float fTimeDelta);

	void Set_Animation(_uint iIndex, _bool isLoop) {
		m_currentAnimIndex = iIndex;
		m_isAnimLoop = isLoop;
	}

	public:
	size_t Get_NumMeshes() { return m_meshes.size(); }
	_int Get_BoneIndex(const _char* pBoneName);

private:
	// 이두개는 내부에서 알아서 메모리 관리 해줌 shared_ptr이런거 필요 x
	// 임포터가 지워질떄 aiScene 도 알아서 지워짐 임포터는 CAssimp_Model이 메모리에서 해제될 때 자동으로 소멸자 호출
	const aiScene* m_AIScene = { nullptr };
	Assimp::Importer m_importer = {};

	EModelType m_type = { EModelType::END };
	_float4x4 m_preLocalTransformMatrix = {};
private:
	_uint m_numMeshes = {};
	vector<shared_ptr<class CAssimp_Mesh>> m_meshes={};

	_uint m_numMaterials = {};
	vector<shared_ptr<class CAssimp_Material>> m_materials={};

	vector< shared_ptr<class CAssimp_Bone>> m_bones = {};

	_bool m_isAnimLoop = { false };
	_uint m_currentAnimIndex = {};
	_uint m_numAnimations = {};
	vector<shared_ptr<class CAssimp_Animation>> m_animations;
public:
	static shared_ptr<CAssimp_Model> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath, EModelType eType, _fmatrix PreLocalTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END