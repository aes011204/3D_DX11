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
	CUITransform* m_pOwnerTransform = { nullptr };

	_float m_fAccTime = {};
	_float m_Range = {};
	_float m_Speed = {};
	_float m_StartPosRad = {};
	_float2 m_BaseScale = {};
	_bool m_Continuous = {};
};

NS_END