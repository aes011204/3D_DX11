#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCollision_Manager :
	public CBase
{
private:
	CCollision_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
\
	virtual ~CCollision_Manager();

public:

	virtual HRESULT Initialize() ;
public:
	void Update(_fmatrix WorldMatrix);
	
	void Add_Collider(shared_ptr<class CCollider> coll);
	void Remove_Collider(shared_ptr<class CCollider> coll);


private:
	vector<shared_ptr<class CCollider>> m_Colliders;

public:
	static unique_ptr<CCollision_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	
	virtual void Free() override;


};

NS_END