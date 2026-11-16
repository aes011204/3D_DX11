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
	void Tick(float deltaTime, CUI* owner) override;

	void OnAppear(float deltaTime, CUI* owner) override;

	void OnDisappear(float deltaTime, CUI* owner) override;

	virtual bool IsFinished() { return m_isFinish; };

private:
	CUITransform* m_ownerTransform = { nullptr };
	_float2 m_startPos = {};
	_float2 m_endPos = {};
	_float m_speed = {};

	_float m_accTime = {};
	_float m_duration={};

	_bool m_isFinish = false;
};

NS_END