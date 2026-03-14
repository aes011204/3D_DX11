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
	virtual HRESULT Initialize_Prototype(MODEL eType, ifstream& InFile, shared_ptr<class CModel> pModel, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);


	_uint Get_MaterialIndex() const {return m_iMaterialIndex;}

	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> shader, const _char* pConstantName, const vector<shared_ptr<CBone>>& bones);

private:
	_uint m_iMaterialIndex = {};

	_uint m_iNumBones = {};
	vector<_uint> m_BoneIndices = {};
	_float4x4 m_BoneMatrices[512] = {};
	vector<_float4x4 > m_OffsetMatrices = {}; // vector인 이유는 뼈갯수 만큼 오프셋 가지고 있음

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(ifstream& InFile, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(ifstream& InFile, shared_ptr<CModel>);
public:
	static shared_ptr<CMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, MODEL eType, ifstream& InFile, shared_ptr<CModel>
	                                pModel, _fmatrix PreTransformMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};


NS_END
