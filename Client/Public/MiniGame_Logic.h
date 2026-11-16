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
	
	struct MiniGameLogicDesc :public CMiniGame::MiniGameDesc
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
	virtual HRESULT Initialize(void* arg);
	virtual HRESULT Render(_uint meshIndex);
	virtual void Update(const _float& timeDelta);

	void ResetZoon();


	//_uint GetInitFishCount() const{return m_FishCount;}
	//float GetProgress01() const { return m_prograssBar01; }

	virtual float GetAngle() const override { return m_angle; }


	virtual const Zone* GetZones() const  override { return m_zones; }
	virtual int GetZoneCount() const override { return m_zoneCount; }


	bool IsStart() const { return m_isStart; }
	bool IsFinish() const { return m_isFinished; }

	//_uint Get_DefID() { return m_DefID; };

	void OnInput()override;


	EMiniGameType Get_MiniGameType() { return m_miniGameType; }
public:
	

	Zone m_zones[8];
	int m_zoneCount;
	float m_accTime = { 0 };
	float m_angle = { 0 };
	float m_speed = {};
	_bool m_isStart = { false };

	float m_zoneSize = {};

	_float2 m_initZoneSize = {};
	_float2 m_initZoneCount = {};
	///
	bool m_changeColor = { false };
	float m_accTime2 = { 0 };
	float m_colorTime = { 0 };

	///
	//float m_prograssBar01 = { 0 };
	//float m_RodSpeed = {};


	//
	//_uint m_DefID = { ID_Absence };
	_bool m_isFinished = { false };

	//
	//_uint m_FishCount = {};
private:
	//weak_ptr<CGameInstance> m_pGameInstance = {};
	//
	//weak_ptr<class CInventory_Controller> m_InvenCtrl = {};
public:
	static shared_ptr<CMiniGame_Logic> Create(void* arg);
	void Free() override;
};

NS_END