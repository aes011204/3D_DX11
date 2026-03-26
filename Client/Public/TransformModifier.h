#pragma once

#include "IModifier.h"
#include "Client_Define.h"
NS_BEGIN(Engine)
class CUITransform;
NS_END

NS_BEGIN(Client)
class CTransformModifier :public IModifier
{
public:
	CTransformModifier(_float2 startPos, _float2 endPos, _float speed, _float Duration);
	virtual ~CTransformModifier();


	// IModifier을(를) 통해 상속됨
	void Tick(float fDeltaTime, CUI* pOwner) override;

	void OnAppear(float fDeltaTime, CUI* pOwner) override;

	void OnDisappear(float fDeltaTime, CUI* pOwner) override;

	virtual bool IsFinished() { return m_bFinish; };

private:
	CUITransform* m_pOwnerTransform = { nullptr };
	_float2 m_startPos = {};
	_float2 m_endPos = {};
	_float m_speed = {};

	_float m_AccTime = {};
	_float m_Duration={};

	_bool m_bFinish = false;
};

NS_END