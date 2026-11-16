#pragma once

#include "IModifier.h"
#include "Client_Define.h"
NS_BEGIN(Engine)
class CUIRenderable;
NS_END
NS_BEGIN(Client)
class CFadeModifier :public IModifier
{
public:
	enum EFade {FADE_OUT, FADE_IN, FADE_END};

public:
	CFadeModifier( EFade fadeType, _float duration, _bool useColor, _float4 color,bool Inactive = true);
	virtual ~CFadeModifier();

	void SetEvent(function<void()> event) { EndEvent = event; };

	// IModifier을(를) 통해 상속됨
	void Tick(float deltaTime, CUI* owner) override;

	void OnAppear(float deltaTime, CUI* owner) override;

	void OnDisappear(float deltaTime, CUI* owner) override;

	virtual bool IsFinished() override { return m_isFinished; };

private:
	function<void()> EndEvent = nullptr;
	CUIRenderable* m_self = {nullptr};

	EFade m_fadeType = FADE_END;
	_float m_duration = {};
	_bool m_useColor = {};
	_float4 m_color = {};

	_float  m_accTime = {0};
	_bool   m_isFinished = false;
	_float  m_alpha = {};

	bool m_isInactive = {};
};
NS_END