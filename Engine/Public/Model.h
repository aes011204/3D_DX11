#pragma once
#include "Component.h"
// 모델은 메쉬로 이루어져있음 
// 컴포넌트로 일단 상속 받음 

NS_BEGIN(Engine)

class ENGINE_DLL CModel final:
    public CComponent
{
protected:
	CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CModel(const CModel& Prototype);
public:
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePat);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

	HRESULT Ready_Meshes();
private:
	// 이두개는 내부에서 알아서 메모리 관리 해줌 shared_ptr이런거 필요 x
	// 임포터가 지워질떄 aiScene 도 알아서 지워짐 임포터는 CModel이 메모리에서 해제될 때 자동으로 소멸자 호출
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer m_Importer = {};

private:
	_uint m_iNumMeshes = {};
	vector<shared_ptr<class CMesh>> m_Meshes;
public:
	static shared_ptr<CModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath);
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

NS_END