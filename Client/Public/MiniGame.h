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

	struct MINIGAEME_L_DESC
	{
		
		_uint DefID = {};
		float RodSpeed = {};
		_uint FishCount = {};

		weak_ptr<class CInventory_Controller> pInvenCtrl;
		MINIGAME MiniGameType = {};
	};
protected:
	CMiniGame();
	//CMiniGame(const CMiniGame& prototype);
public:
	~CMiniGame() = default;

public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);
	virtual void Update(const _float& timeDelta);
	_uint SelectMutationRandom(const Fish_Def& fishDef);
	_uint SelectMutationNoRepeat(const Fish_Def& fishDef);

	INPUT_RESULT ConsumeInputResult() {
		INPUT_RESULT result = m_LastInputResult;
		m_LastInputResult = INPUT_RESULT::NONE;
		return result;
	};
	virtual void OnInput() = 0;

	//void RequestInput(){  m_bInputRequested = true;};

	//bool IsStart() const { return m_bStart; }
	//bool IsFinish() const { return m_bFin; }

	MINIGAME Get_MiniGameType() { return m_MiniGameType; }
	_uint Get_DefID() { return m_DefID; };
	_uint GetFishCount() const { return m_FishCount; }
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
	INPUT_RESULT m_LastInputResult = INPUT_RESULT::NONE;

	_bool m_bInputRequested = false;

	_uint m_DefID = { ID_Absence };
	float m_RodSpeed = {};
	_uint m_FishCount = {};
	float m_prograssBar01 = { 0 };

	MINIGAME m_MiniGameType = {};
	weak_ptr<class CInventory_Controller> m_InvenCtrl = {};
	weak_ptr<CGameInstance> m_pGameInstance = {};
	_uint m_PrevFishID = { ID_Absence };
public:
//	static shared_ptr<CMiniGame> Create(void* pArg);
	void Free() override;
};

NS_END