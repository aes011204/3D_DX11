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
	CAssimp_Model(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CAssimp_Model(const CAssimp_Model& prototype);
public:
	virtual ~CAssimp_Model() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* modelFilePath, EModelType type, _fmatrix preLocalTransformMatrix);
	virtual HRESULT Initialize(void* arg);
	virtual HRESULT Render(_uint meshIndex);

	HRESULT Ready_Meshes(EModelType type);

	HRESULT Ready_Material(const _char* modelFilePath);
	HRESULT Bind_Material(shared_ptr<CShader> shader, const _char* constantName, _uint meshIndex,
	                      aiTextureType materialType, _uint textureIndex);

	HRESULT Ready_Bones(const aiNode* srcAINode, _int parentIndex);
	HRESULT Bind_BoneMatrices(shared_ptr<CShader> shader, const _char* constantNamem, _uint meshIndex);

	HRESULT Ready_Animations();
	HRESULT Play_Animation(_float timeDelta);

	void Set_Animation(_uint index, _bool isLoop) {
		m_currentAnimIndex = index;
		m_isAnimLoop = isLoop;
	}

	public:
	size_t Get_NumMeshes() { return m_meshes.size(); }
	_int Get_BoneIndex(const _char* boneName);

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
	static shared_ptr<CAssimp_Model> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _char* modelFilePath, EModelType type, _fmatrix preLocalTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;
};

NS_END