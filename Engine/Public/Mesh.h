#pragma once
#include "Bone.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)

	class CMesh final:
    public CVIBuffer
{
private:
	CMesh(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CMesh(const CMesh& prototype);
public:
	virtual ~CMesh() = default;



public:
	virtual HRESULT Initialize_Prototype(EModelType type, ifstream& InFile, shared_ptr<class CModel> model, _fmatrix preTransformMatrix);
	virtual HRESULT Initialize(void* arg);


	_uint Get_MaterialIndex() const {return m_materialIndex;}

	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> shader, const _char* constantName, const vector<shared_ptr<CBone>>& bones);

private:
	_char m_name[MAX_PATH] = {};

	_uint m_materialIndex = {};

	_uint m_numBones = {};
	vector<_uint> m_boneIndices = {};
	_float4x4 m_boneMatrices[512] = {};
	vector<_float4x4 > m_offsetMatrices = {}; // vector인 이유는 뼈갯수 만큼 오프셋 가지고 있음

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(ifstream& InFile, _fmatrix preTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(ifstream& InFile, shared_ptr<CModel>);
public:
	static shared_ptr<CMesh> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, EModelType type, ifstream& InFile, shared_ptr<class CModel>
	                                model, _fmatrix preTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* arg);
	virtual void Free() override;
};


NS_END
