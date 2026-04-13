#pragma once
#include "Base.h"
#include "Client_Enum.h"


namespace Engine
{
	class CGameInstance;
}

NS_BEGIN(Client)

	class CMiniGame_Logic :
	public CBase
{
public:
	enum MINIGAME { BASIC_CIRCLE, END };
	struct MINIGAEMELOGIC_DESC 
	{
		int zoneCount = {};
		_float2 zoneSize = {};
		_uint DefID = {};
		float RodSpeed = {};
		_uint FishCount = {};
	};
protected:
	CMiniGame_Logic();
	CMiniGame_Logic(const CMiniGame_Logic& prototype);
public:
	~CMiniGame_Logic() = default;

public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);
	virtual void Update(const _float& timeDelta);

	void Set_InvenCtrl(shared_ptr<class CInventory_Controller> invenCtrl) {
		m_InvenCtrl = invenCtrl
			;
	}




	float GetProgress01() const { return m_prograssBar01; }

	float GetAngle() const { return m_Angle; }


	const Zone* GetZones() const { return m_zones; }
	int GetZoneCount() const { return m_zoneCount; }


	bool IsStart() const { return m_bStart; }
	bool IsFinish() const { return m_bFin; }



public:
	Zone m_zones[8];
	float m_zoneCount;
	float m_AccTime = { 0 };
	float m_Angle = { 0 };
	float m_Speed = {};
	_bool m_bStart = { false };

	float m_zoneSize = {};

	///
	bool m_changeColor = { false };
	float m_AccTime2 = { 0 };
	float colortime = { 0 };

	///
	float m_prograssBar01 = { 0 };
	float m_RodSpeed = {};


	//
	_uint m_DefID = { ID_Absence };
	_bool m_bFin = { false };

	//
	_uint m_FishCount = {};
private:
	weak_ptr<CGameInstance> m_pGameInstance = {};
	//
	weak_ptr<class CInventory_Controller> m_InvenCtrl = {};
public:
	static shared_ptr<CMiniGame_Logic> Create();
	void Free() override;
};

NS_END