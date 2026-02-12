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
	LARGE_INTEGER m_FrameTime = {};
	LARGE_INTEGER m_FixTime = {};
	LARGE_INTEGER m_LastTime = {};
	LARGE_INTEGER m_CpuTick = {};

	_float m_fTimeDelta = {};

public:
	static shared_ptr<CTimer> Create();

public:
	void Free() override;
};

NS_END
