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
	CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CModel(const CModel& Prototype);
public:
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

	HRESULT Ready_Meshes(MODEL eType, ifstream& InFile);

	HRESULT Ready_Material(ifstream& InFile);
	HRESULT Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex,
		Cvt_TexType eMaterialType, _uint iTextureIndex);

	HRESULT Ready_Bones(ifstream& InFile);

	//HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantNamem, _uint iMeshIndex);

	HRESULT Ready_Animations(ifstream& InFile);


	//HRESULT Ready_Animations(); // 각 뼈들이 시간에 따라서 어떤 상태를 띈다.


	HRESULT Play_Animation(_float fTimeDelta);

	void Set_Animation(_uint iIndex, _bool isLoop);
	public:
	size_t Get_NumMeshes() { return m_Meshes.size(); }
	_int Get_BoneIndex(const _char* pBoneName);
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);
	_bool Check_AnimFrame(_int targetFrame);
private:
	// 이두개는 내부에서 알아서 메모리 관리 해줌 shared_ptr이런거 필요 x
	// 임포터가 지워질떄 aiScene 도 알아서 지워짐 임포터는 CModel이 메모리에서 해제될 때 자동으로 소멸자 호출
	/*const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer = {};*/

	MODEL m_eType = { MODEL::END };
	_float4x4 m_PreLocalTransformMatrix = {};
private:
	_uint m_iNumMeshes = {};
	vector<shared_ptr<class CMesh>> m_Meshes ={};

	_uint m_iNumMaterials = {};
	vector<shared_ptr<class CMaterial>> m_Materials={};

	_uint m_iTotalNumBone = {};
	vector< shared_ptr<class CBone>> m_Bones = {};

	_bool m_isAnimLoop = { false };
	_uint m_iCurrentAnimIndex = {};
	_uint m_BlendIndex = {};
	_uint m_iNumAnimations = {};
	_bool m_IsBlending = {};
	_float m_fBlendTime = {1.f};
	_float m_fBlendElapsed = {};
	_bool m_BlendIsLoop = {};
	vector<shared_ptr<class CAnimation>> m_Animations;



	public:
		const _bool Get_IsFinishAnim() { return m_isFinish; }
		_int Get_CurrentFrame() const;
		
	private:
		_bool m_isFinish = false;

	private:
	_int m_iPrevFrame = 0;
	
public:
	static shared_ptr<CModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END