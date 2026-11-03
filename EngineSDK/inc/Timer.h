#pragma once
#include "Base.h"
#include "Engine_Define.h"

NS_BEGIN(Engine)

class CTimer : public CBase
{
private:
	CTimer();
public:
	~CTimer() = default;

public:
	HRESULT Ready_Timer();
	_float Update_Timer();

private:
	LARGE_INTEGER m_frameTime = {};
	LARGE_INTEGER m_fixTime = {};
	LARGE_INTEGER m_lastTime = {};
	LARGE_INTEGER m_cpuTick = {};

	_float m_timeDelta = {};

public:
	static shared_ptr<CTimer> Create();

public:
	void Free() override;
};

NS_END
