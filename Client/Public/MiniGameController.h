#pragma once
#include "Client_Define.h"
#include "Base.h"


namespace Engine
{
	class CGameInstance;
}

NS_BEGIN(Client)

class CInventory_Controller;

	class CFish;

	class CMiniGameController :
	public CBase
{
public:

protected:
	CMiniGameController();
	CMiniGameController(const CMiniGameController& prototype);
public:
	~CMiniGameController() = default;

public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* arg);
	virtual HRESULT Render(_uint meshIndex);
	virtual void Update(const _float& timeDelta);

	void Start_Fishing(weak_ptr<CFish> fish, weak_ptr<CInventory_Controller> invenCtrl, float rodSpeed);


public:
	
private:
	weak_ptr<CGameInstance> m_gameInstance = {};
	//
	//weak_ptr<class CInventory_Controller> m_InvenCtrl = {}; 
public:
	static shared_ptr<CMiniGameController> Create();
	void Free() override;
};

NS_END