#pragma once
#include "Assimp_Bone.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)

	class CAssimp_Mesh final:
    public CVIBuffer
{
private:
	CAssimp_Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAssimp_Mesh(const CAssimp_Mesh& Prototype);
public:
	virtual ~CAssimp_Mesh() = default;



public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, shared_ptr <class CAssimp_Model > pModel, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);


	_uint Get_MaterialIndex() const {return m_iMaterialIndex;}

	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> shader, const _char* pConstantName, const vector<shared_ptr<CAssimp_Bone>>& bones);

private:
	_uint m_iMaterialIndex = {};

	_uint m_iNumBones = {};
	vector<_uint> m_BoneIndices = {};
	_float4x4 m_BoneMatrices[512] = {};
	vector<_float4x4 > m_OffsetMatrices = {}; // vector인 이유는 뼈갯수 만큼 오프셋 가지고 있음

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, shared_ptr<class CAssimp_Model>);
public:
	static shared_ptr<CAssimp_Mesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, MODEL eType, const aiMesh* pAIMesh, shared_ptr<CAssimp_Model> pModel, _fmatrix PreTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};


NS_END
