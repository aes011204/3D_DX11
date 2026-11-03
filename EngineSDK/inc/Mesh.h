#pragma once
#include "Bone.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)

	class CMesh final:
    public CVIBuffer
{
private:
	CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMesh(const CMesh& Prototype);
public:
	virtual ~CMesh() = default;



public:
	virtual HRESULT Initialize_Prototype(EModelType eType, ifstream& InFile, shared_ptr<class CModel> pModel, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);


	_uint Get_MaterialIndex() const {return m_materialIndex;}

	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> shader, const _char* pConstantName, const vector<shared_ptr<CBone>>& bones);

private:
	_char m_name[MAX_PATH] = {};

	_uint m_materialIndex = {};

	_uint m_numBones = {};
	vector<_uint> m_boneIndices = {};
	_float4x4 m_boneMatrices[512] = {};
	vector<_float4x4 > m_offsetMatrices = {}; // vector인 이유는 뼈갯수 만큼 오프셋 가지고 있음

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(ifstream& InFile, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(ifstream& InFile, shared_ptr<CModel>);
public:
	static shared_ptr<CMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, EModelType eType, ifstream& InFile, shared_ptr<class CModel>
	                                pModel, _fmatrix PreTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};


NS_END
