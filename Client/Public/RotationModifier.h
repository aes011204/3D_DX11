#pragma once
#include "IModifier.h"

#include "Client_Define.h"
NS_BEGIN(Engine)
class CUITransform;
NS_END

NS_BEGIN(Client)
class CRotationModifier :
	public IModifier
{
public:
	CRotationModifier(_float speed);
	virtual ~CRotationModifier();


	// IModifier을(를) 통해 상속됨
	void Tick(float fDeltaTime, CUI* pOwner) override;

	void OnAppear(float fDeltaTime, CUI* pOwner) override;

	void OnDisappear(float fDeltaTime, CUI* pOwner) override;

	virtual bool IsFinished() { return false; };

private:
	CUITransform* m_ownerTransform = { nullptr };

	_float m_accTime = {};
	_float m_range = {};
	_float m_speed = {};
	_float m_startPosRad = {};
	_float2 m_baseScale = {};
	_bool m_isContinuous = {};
};

NS_END