#pragma once

#include "Client_Enum.h"
#include "MiniGame.h"


namespace Engine
{
	class CGameInstance;
}

NS_BEGIN(Client)

	class CMiniGame_Logic_Diamond :
	public CMiniGame
{
public:
	
	struct MiniGameLogicDiamondDesc :public CMiniGame::MiniGameDesc
	{
		//_float Speed = {};
	};
protected:
	CMiniGame_Logic_Diamond();
	CMiniGame_Logic_Diamond(const CMiniGame_Logic_Diamond& prototype);
public:
	~CMiniGame_Logic_Diamond() = default;

public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);
	virtual void Update(const _float& timeDelta);

	
	virtual float GetAngle() const override { return m_angle; }

	float GetSize() const override{return m_size;}


	//bool IsStart() const { return m_bStart; }
	//bool IsFinish() const { return m_bFin; }

	//virtual float GetAngle() const { return m_Angle; }
	void OnInput();

public:
	

	
	
	float m_accTime = { 0 };
	float m_speed = {};
	_bool m_isStart = { false };

	float m_size = {0};

	float m_angle = {};

	///
	bool m_changeColor = { false };
	float m_accTime2 = { 0 };
	float colortime = { 0 };



	_bool m_isFinished = { false };


private:

public:
	static shared_ptr<CMiniGame_Logic_Diamond> Create(void* pArg);
	void Free() override;
};

NS_END