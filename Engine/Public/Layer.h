#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLayer :
    public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	void Update_Gui();

private:
	list <class CGameObject*> m_pGameObject = {};

public:
	static CLayer* Create();
	virtual void Free() override;

};

NS_END