#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLayer :
    public CBase
{
private:
	CLayer();
public:
	virtual ~CLayer() = default;
public:
	HRESULT Add_GameObject(shared_ptr<class CGameObject> pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	void Update_Gui();

	shared_ptr<CGameObject> Get_GameObject(_uint GObjIndex);

/// <IMGUI>
	list <shared_ptr<class CGameObject>> Get_GameObject();
/// </summary>
private:
	list <shared_ptr<class CGameObject>> m_pGameObject = {};

public:
	static shared_ptr<CLayer> Create();
	virtual void Free() override;

};

NS_END