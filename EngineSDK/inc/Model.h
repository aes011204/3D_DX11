#pragma once
#include "Component.h"
// 모델은 메쉬로 이루어져있음 
// 컴포넌트로 일단 상속 받음 

NS_BEGIN(Engine)

class CShader;

	class ENGINE_DLL CModel final:
    public CComponent
{
protected:
	CModel(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CModel(const CModel& prototype);
public:
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* modelFilePath, EModelType type, _fmatrix preLocalTransformMatrix);
	virtual HRESULT Initialize(void* arg);
	virtual HRESULT Render(_uint meshIndex);

	HRESULT Ready_Meshes(EModelType type, ifstream& InFile);

	HRESULT Ready_Material(ifstream& InFile);
	HRESULT Bind_Material(shared_ptr<CShader> shader, const _char* constantName, _uint meshIndex,
		Cvt_TexType materialType, _uint textureIndex);

	HRESULT Ready_Bones(ifstream& InFile);

	//HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Bind_BoneMatrices(shared_ptr<CShader> shader, const _char* constantNamem, _uint meshIndex);

	HRESULT Ready_Animations(ifstream& InFile);


	//HRESULT Ready_Animations(); // 각 뼈들이 시간에 따라서 어떤 상태를 띈다.


	HRESULT Play_Animation(_float timeDelta);

	void Set_Animation(_uint index, _bool isLoop);
	public:
	size_t Get_NumMeshes() { return m_meshes.size(); }
	_int Get_BoneIndex(const _char* boneName);
	const _float4x4* Get_BoneMatrixPtr(const _char* boneName);
	_bool Check_AnimFrame(_int targetFrame);
private:
	// 이두개는 내부에서 알아서 메모리 관리 해줌 shared_ptr이런거 필요 x
	// 임포터가 지워질떄 aiScene 도 알아서 지워짐 임포터는 CModel이 메모리에서 해제될 때 자동으로 소멸자 호출
	/*const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer = {};*/

	EModelType m_type = { EModelType::END };
	_float4x4 m_preLocalTransformMatrix = {};
private:
	_uint m_numMeshes = {};
	vector<shared_ptr<class CMesh>> m_meshes ={};

	_uint m_numMaterials = {};
	vector<shared_ptr<class CMaterial>> m_materials={};

	_uint m_totalNumBone = {};
	vector< shared_ptr<class CBone>> m_bones = {};

	_bool m_isAnimLoop = { false };
	_uint m_currentAnimIndex = {};
	_uint m_blendIndex = {};
	_uint m_numAnimations = {};
	_bool m_isBlending = {};
	_float m_blendTime = {1.f};
	_float m_blendElapsed = {};
	_bool m_isBlendLoop = {};
	vector<shared_ptr<class CAnimation>> m_animations;



	public:
		const _bool Get_IsFinishAnim() { return m_isFinish; }
		_int Get_CurrentFrame() const;
		
	private:
		_bool m_isFinish = false;

	private:
	_int m_prevFrame = 0;
	
public:
	static shared_ptr<CModel> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _char* modelFilePath, EModelType type, _fmatrix preLocalTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;
};

NS_END