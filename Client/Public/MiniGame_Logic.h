#pragma once

#include "Client_Enum.h"
#include "MiniGame.h"


namespace Engine
{
	class CGameInstance;
}

NS_BEGIN(Client)

	class CMiniGame_Logic :
	public CMiniGame
{
public:
	
	struct MINIGAEMELOGIC_DESC :public CMiniGame::MINIGAEME_L_DESC
	{
		_float2 zoneCount = {};
		_float2 zoneSize = {};
	

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

	void ResetZoon();


	//_uint GetInitFishCount() const{return m_FishCount;}
	//float GetProgress01() const { return m_prograssBar01; }

	virtual float GetAngle() const override { return m_Angle; }


	virtual const Zone* GetZones() const  override { return m_zones; }
	virtual int GetZoneCount() const override { return m_zoneCount; }


	bool IsStart() const { return m_bStart; }
	bool IsFinish() const { return m_bFin; }

	//_uint Get_DefID() { return m_DefID; };

	void OnInput()override;


	MINIGAME Get_MiniGameType() { return m_MiniGameType; }
public:
	

	Zone m_zones[8];
	int m_zoneCount;
	float m_AccTime = { 0 };
	float m_Angle = { 0 };
	float m_Speed = {};
	_bool m_bStart = { false };

	float m_zoneSize = {};

	_float2 m_InitZoonSize = {};
	_float2 m_InitZoonCount = {};
	///
	bool m_changeColor = { false };
	float m_AccTime2 = { 0 };
	float colortime = { 0 };

	///
	//float m_prograssBar01 = { 0 };
	//float m_RodSpeed = {};


	//
	//_uint m_DefID = { ID_Absence };
	_bool m_bFin = { false };

	//
	//_uint m_FishCount = {};
private:
	//weak_ptr<CGameInstance> m_pGameInstance = {};
	//
	//weak_ptr<class CInventory_Controller> m_InvenCtrl = {};
public:
	static shared_ptr<CMiniGame_Logic> Create(void* pArg);
	void Free() override;
};

NS_END