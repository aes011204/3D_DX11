#pragma once
#include "Base.h"
#include "Client_Enum.h"

namespace Engine
{
	class CGameInstance;
}

NS_BEGIN(Client)

class CMiniGame :
	public CBase
{
public:

	struct MiniGameDesc
	{
		
		_uint defID = {};
		float rodSpeed = {};
		_uint fishCount = {};

		weak_ptr<class CInventory_Controller> invenCtrl;
		EMiniGameType miniGameType = {};
	};
protected:
	CMiniGame();
	//CMiniGame(const CMiniGame& prototype);
public:
	~CMiniGame() = default;

public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* arg);
	virtual HRESULT Render(_uint meshIndex);
	virtual void Update(const _float& timeDelta);
	_uint SelectMutationRandom(const FishDef& fishDef);
	_uint SelectMutationNoRepeat(const FishDef& fishDef);

	EInputResult ConsumeInputResult() {
		EInputResult result = m_lastInputResult;
		m_lastInputResult = EInputResult::NONE;
		return result;
	};
	virtual void OnInput() = 0;

	//void RequestInput(){  m_bInputRequested = true;};

	//bool IsStart() const { return m_bStart; }
	//bool IsFinish() const { return m_bFin; }

	EMiniGameType Get_MiniGameType() { return m_miniGameType; }
	_uint Get_DefID() { return m_defID; };
	_uint GetFishCount() const { return m_fishCount; }
	float GetProgress01() const { return m_prograssBar01; }



	/// circle
	virtual float GetAngle() const { return 0.f; }
	virtual const Zone* GetZones() const { return nullptr; }
	virtual int GetZoneCount() const { return 0; }
	///

	/// diamond
	virtual float GetSize() const { return 0.f; }
	///
	///
protected:
	EInputResult m_lastInputResult = EInputResult::NONE;

	_bool m_isInputRequested = false;

	_uint m_defID = { kIDAbsence };
	float m_rodSpeed = {};
	_uint m_fishCount = {};
	float m_prograssBar01 = { 0 };

	EMiniGameType m_miniGameType = {};
	weak_ptr<class CInventory_Controller> m_invenCtrl = {};
	weak_ptr<CGameInstance> m_gameInstance = {};
	_uint m_prevFishID = { kIDAbsence };
public:
//	static shared_ptr<CMiniGame> Create(void* pArg);
	void Free() override;
};

NS_END