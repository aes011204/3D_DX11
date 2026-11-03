#pragma once
#include "Assimp_Bone.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)

	class CAssimp_Mesh final:
    public CVIBuffer
{
private:
	CAssimp_Mesh(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CAssimp_Mesh(const CAssimp_Mesh& prototype);
public:
	virtual ~CAssimp_Mesh() = default;



public:
	virtual HRESULT Initialize_Prototype(EModelType type, const aiMesh* srcAIMesh, shared_ptr <class CAssimp_Model > model, _fmatrix preTransformMatrix);
	virtual HRESULT Initialize(void* arg);


	_uint Get_MaterialIndex() const {return m_materialIndex;}

	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> shader, const _char* constantName, const vector<shared_ptr<CAssimp_Bone>>& bones);

private:
	_uint m_materialIndex = {};

	_uint m_numBones = {};
	vector<_uint> m_boneIndices = {};
	_float4x4 m_boneMatrices[512] = {};
	vector<_float4x4 > m_offsetMatrices = {}; // vector인 이유는 뼈갯수 만큼 오프셋 가지고 있음

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* srcAIMesh, _fmatrix preTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* srcAIMesh, shared_ptr<class CAssimp_Model>);
public:
	static shared_ptr<CAssimp_Mesh> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, EModelType type, const aiMesh* srcAIMesh, shared_ptr<CAssimp_Model> model, _fmatrix preTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;
};


NS_END
