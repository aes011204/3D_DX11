#pragma once
#include "Client_Enum.h"
#include "MiniGame.h"


namespace Engine
{
	class CGameInstance;
}

NS_BEGIN(Client)

	class CMiniGame_Logic_Ball :
	public CMiniGame
{
public:
	
	struct MiniGameLogicBallDesc 
	{
		_uint CallNum = {};
			_float2 Speed = {};


	};
protected:
	CMiniGame_Logic_Ball();
	CMiniGame_Logic_Ball(const CMiniGame_Logic_Ball& prototype);
public:
	~CMiniGame_Logic_Ball() = default;

public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);
	virtual void Update(const _float& timeDelta);

	void ResetZoon();


	_uint GetFishCount() const{return m_fishCount;}
	float GetProgress01() const { return m_prograssBar01; }

	float GetAngle() const { return m_angle; }


	const Zone* GetZones() const { return m_zones; }
	int GetZoneCount() const { return m_zoneCount; }


	bool IsStart() const { return m_isStart; }
	bool IsFinish() const { return m_isFinished; }

	_uint Get_DefID() { return m_defId; };

	void OnInput();

	EInputResult ConsumeInputResult() {
		EInputResult result = m_lastInputResult;
		m_lastInputResult = EInputResult::NONE;
		return result;
	}
	EMiniGameType Get_MiniGameType() { return m_miniGameType; }
public:

	EInputResult m_lastInputResult = EInputResult::NONE;

	Zone m_zones[8];
	int m_zoneCount;
	float m_accTime = { 0 };
	float m_angle = { 0 };
	float m_speed = {};
	_bool m_isStart = { false };

	float m_zoneSize = {};

	_float2 m_initZoneSize = {};

	///
	bool m_changeColor = { false };
	float m_accTime2 = { 0 };
	float colortime = { 0 };

	///
	float m_prograssBar01 = { 0 };




	_bool m_isFinished = { false };

	
private:

public:
	static shared_ptr<CMiniGame_Logic_Ball> Create(void* pArg);
	void Free() override;
};

NS_END