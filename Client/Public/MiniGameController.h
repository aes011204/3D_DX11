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
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);
	virtual void Update(const _float& timeDelta);

	void Start_Fishing(weak_ptr<CFish> m_pFish, weak_ptr<CInventory_Controller> InvenCtrl, float RodSpeeed);


public:
	
private:
	weak_ptr<CGameInstance> m_pGameInstance = {};
	//
	//weak_ptr<class CInventory_Controller> m_InvenCtrl = {}; 
public:
	static shared_ptr<CMiniGameController> Create();
	void Free() override;
};

NS_END