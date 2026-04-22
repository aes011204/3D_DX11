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
	
	struct MINIGAEMELOGIC_BALL_DESC 
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


	_uint GetFishCount() const{return m_FishCount;}
	float GetProgress01() const { return m_prograssBar01; }

	float GetAngle() const { return m_Angle; }


	const Zone* GetZones() const { return m_zones; }
	int GetZoneCount() const { return m_zoneCount; }


	bool IsStart() const { return m_bStart; }
	bool IsFinish() const { return m_bFin; }

	_uint Get_DefID() { return m_DefID; };

	void OnInput();

	INPUT_RESULT ConsumeInputResult() {
		INPUT_RESULT result = m_LastInputResult;
		m_LastInputResult = INPUT_RESULT::NONE;
		return result;
	}
	MINIGAME Get_MiniGameType() { return m_MiniGameType; }
public:

	INPUT_RESULT m_LastInputResult = INPUT_RESULT::NONE;

	Zone m_zones[8];
	int m_zoneCount;
	float m_AccTime = { 0 };
	float m_Angle = { 0 };
	float m_Speed = {};
	_bool m_bStart = { false };

	float m_zoneSize = {};

	_float2 m_InitZoonSize = {};

	///
	bool m_changeColor = { false };
	float m_AccTime2 = { 0 };
	float colortime = { 0 };

	///
	float m_prograssBar01 = { 0 };




	_bool m_bFin = { false };

	
private:

public:
	static shared_ptr<CMiniGame_Logic_Ball> Create(void* pArg);
	void Free() override;
};

NS_END