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

	HRESULT Ready_Meshes();

	HRESULT Ready_Material(const _char* pModelFilePath);
	HRESULT Bind_Material(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex,
	                      aiTextureType eMaterialType, _uint iTextureIndex);
	

	size_t Get_NumMeshes() { return m_Meshes.size(); }


private:
	// 이두개는 내부에서 알아서 메모리 관리 해줌 shared_ptr이런거 필요 x
	// 임포터가 지워질떄 aiScene 도 알아서 지워짐 임포터는 CModel이 메모리에서 해제될 때 자동으로 소멸자 호출
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer = {};

	_float4x4 m_PreLocalTransformMatrix = {};
	MODEL m_eType = { MODEL::END };
private:
	_uint m_iNumMeshes = {};
	vector<shared_ptr<class CMesh>> m_Meshes;

	_uint m_iNumMaterials = {};
	vector<shared_ptr<class CMaterial>> m_Materials;
public:
	static shared_ptr<CModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath, MODEL eType, _fmatrix PreLocalTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END